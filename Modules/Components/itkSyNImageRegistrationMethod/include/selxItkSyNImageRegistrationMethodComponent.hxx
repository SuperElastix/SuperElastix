/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxItkSyNImageRegistrationMethodComponent.h"
#include "selxItkImageRegistrationMethodv4Component.h"

#include "itkDisplacementFieldTransformParametersAdaptor.h"
//TODO: get rid of these
#include "itkGradientDescentOptimizerv4.h"

namespace selx
{
template< int Dimensionality, class TPixel , class InternalComputationValueType >
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
  ::ItkSyNImageRegistrationMethodComponent( const std::string & name, LoggerImpl & logger ) 
  : Superclass( name, logger )
{
  m_SyNImageRegistrationMethod = SyNImageRegistrationMethodType::New();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >::~ItkSyNImageRegistrationMethodComponent()
{
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
::Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_FixedImage = component->GetItkImageFixed();
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_MovingImage = component->GetItkImageMoving();
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
::Accept( typename itkMetricv4Interface< Dimensionality, TPixel, InternalComputationValueType >::Pointer component )
{
  this->m_SyNImageRegistrationMethod->SetMetric( component->GetItkMetricv4() );

  return 0;
}

template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >::BeforeUpdate( void )
{

};

template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >::Update( void )
{
	this->m_FixedImage->Update();
	this->m_SyNImageRegistrationMethod->SetFixedImage(this->m_FixedImage);

	this->m_FixedImage->Update();
	this->m_SyNImageRegistrationMethod->SetMovingImage(this->m_MovingImage);

	typedef itk::Vector< InternalComputationValueType, Dimensionality > VectorType;
	VectorType zeroVector( 0.0 );

	typedef itk::Image< VectorType, Dimensionality > DisplacementFieldType;
	typename DisplacementFieldType::Pointer displacementField = DisplacementFieldType::New();
	displacementField->CopyInformation( this->m_FixedImage );
	displacementField->SetRegions( this->m_FixedImage->GetBufferedRegion() );
	displacementField->Allocate();
	displacementField->FillBuffer( zeroVector );

	typename DisplacementFieldType::Pointer inverseDisplacementField = DisplacementFieldType::New();
	inverseDisplacementField->CopyInformation( this->m_FixedImage );
	inverseDisplacementField->SetRegions( this->m_FixedImage->GetBufferedRegion() );
	inverseDisplacementField->Allocate();
	inverseDisplacementField->FillBuffer( zeroVector );

	typedef typename SyNImageRegistrationMethodType::OutputTransformType OutputTransformType;
	typename OutputTransformType::Pointer outputTransform = OutputTransformType::New();
	outputTransform->SetDisplacementField( displacementField );
	outputTransform->SetInverseDisplacementField( inverseDisplacementField );

	this->m_SyNImageRegistrationMethod->SetInitialTransform( outputTransform );

  // Scale estimator is not used in current implementation yet
  typename ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();

  ImageMetricType * theMetric = dynamic_cast< ImageMetricType * >( this->m_SyNImageRegistrationMethod->GetModifiableMetric() );

  auto optimizer = dynamic_cast< itk::GradientDescentOptimizerv4 * >( this->m_SyNImageRegistrationMethod->GetModifiableOptimizer() );
  //auto optimizer = dynamic_cast<itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > *>(this->m_SyNImageRegistrationMethod->GetModifiableOptimizer());

  if( theMetric )
  {
    scalesEstimator->SetMetric( theMetric );
  }
  else
  {
    throw std::runtime_error( "Error casting to ImageMetricv4Type failed" );
  }

  scalesEstimator->SetTransformForward( true );
  scalesEstimator->SetSmallParameterVariation( 1.0 );

  optimizer->SetScalesEstimator( ITK_NULLPTR );
  //optimizer->SetScalesEstimator(scalesEstimator);
  optimizer->SetDoEstimateLearningRateOnce( false ); //true by default
  optimizer->SetDoEstimateLearningRateAtEachIteration( false );

	typedef typename SyNImageRegistrationMethodType::OutputTransformType OutputTransformType;
  typedef itk::DisplacementFieldTransformParametersAdaptor< OutputTransformType > DisplacementFieldTransformAdaptorType;

  typename SyNImageRegistrationMethodType::TransformParametersAdaptorsContainerType adaptors;

  auto numberOfResolutionLevels = this->m_SyNImageRegistrationMethod->GetNumberOfLevels();
  
  for( unsigned int level = 0; level < numberOfResolutionLevels; level++ )
  {
    // We use the shrink image filter to calculate the fixed parameters of the virtual
    // domain at each level.  To speed up calculation and avoid unnecessary memory
    // usage, we could calculate these fixed parameters directly.

    typedef itk::ShrinkImageFilter< FixedImageType, FixedImageType > ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
    shrinkFilter->SetShrinkFactors( this->m_SyNImageRegistrationMethod->GetShrinkFactorsPerDimension( level ) );
    shrinkFilter->SetInput( this->m_FixedImage );
    shrinkFilter->Update();

    typename DisplacementFieldTransformAdaptorType::Pointer fieldTransformAdaptor = DisplacementFieldTransformAdaptorType::New();
    fieldTransformAdaptor->SetRequiredSpacing( shrinkFilter->GetOutput()->GetSpacing() );
    fieldTransformAdaptor->SetRequiredSize( shrinkFilter->GetOutput()->GetBufferedRegion().GetSize() );
    fieldTransformAdaptor->SetRequiredDirection( shrinkFilter->GetOutput()->GetDirection() );
    fieldTransformAdaptor->SetRequiredOrigin( shrinkFilter->GetOutput()->GetOrigin() );

    adaptors.push_back( fieldTransformAdaptor.GetPointer() );
  }

  this->m_SyNImageRegistrationMethod->SetTransformParametersAdaptorsPerLevel( adaptors );

  typedef CommandIterationUpdate< SyNImageRegistrationMethodType > RegistrationCommandType;
  typename RegistrationCommandType::Pointer registrationObserver = RegistrationCommandType::New();
  this->m_SyNImageRegistrationMethod->AddObserver( itk::IterationEvent(), registrationObserver );

  // perform the actual registration
  this->m_SyNImageRegistrationMethod->Update();
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
typename ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >::TransformPointer
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
::GetItkTransform()
{
  return this->m_SyNImageRegistrationMethod->GetModifiableTransform();
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
bool
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
	bool hasUndefinedCriteria(false);
	bool meetsCriteria(false);

	// First check if user-provided properties are template properties and if this component was instantiated with those template properties.
	auto status = CheckTemplateProperties(this->TemplateProperties(), criterion);
	if (status == CriterionStatus::Satisfied)
	{
		return true;
	}
	else if (status == CriterionStatus::Failed)
	{
		return false;
	} // else: CriterionStatus::Unknown

	// Next else-if blocks check if the name of setting is an existing property for this component, otherwise MeetsCriterion returns CriterionStatus::Failed.
	else if (criterion.first == "NumberOfLevels") //Does the Components have this setting?
	{
		meetsCriteria = true;
		if (criterion.second.size() == 1)
		{
			if (this->m_NumberOfLevelsLastSetBy == "") // check if some other settings set the NumberOfLevels
			{
				// try catch?
				this->m_SyNImageRegistrationMethod->SetNumberOfLevels(std::stoi(criterion.second[0]));
				this->m_NumberOfLevelsLastSetBy = criterion.first;
			}
			else
			{
				if (this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != std::stoi(criterion.second[0]))
				{
					// TODO log error?
					std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
					meetsCriteria = false;
					return meetsCriteria;
				}
			}
		}
		else
		{
			// TODO log error?
			std::cout << "NumberOfLevels accepts one number only" << std::endl;
			meetsCriteria = false;
			return meetsCriteria;
		}
	}
	else if (criterion.first == "ShrinkFactorsPerLevel") //Supports this?
	{
		meetsCriteria = true;

		const int impliedNumberOfResolutions = criterion.second.size();

		if (this->m_NumberOfLevelsLastSetBy == "") // check if some other settings set the NumberOfLevels
		{
			// try catch?
			this->m_SyNImageRegistrationMethod->SetNumberOfLevels(impliedNumberOfResolutions);
			this->m_NumberOfLevelsLastSetBy = criterion.first;
		}
		else
		{
			if (this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != impliedNumberOfResolutions)
			{
				// TODO log error?
				std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
				meetsCriteria = false;
				return meetsCriteria;
			}
		}

		itk::Array< itk::SizeValueType > shrinkFactorsPerLevel;
		shrinkFactorsPerLevel.SetSize(impliedNumberOfResolutions);

		unsigned int resolutionIndex = 0;
		for (auto const & criterionValue : criterion.second) // auto&& preferred?
		{
			shrinkFactorsPerLevel[resolutionIndex] = std::stoi(criterionValue);
			++resolutionIndex;
		}
		// try catch?
		this->m_SyNImageRegistrationMethod->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
	}
	else if (criterion.first == "SmoothingSigmasPerLevel") //Supports this?
	{
		meetsCriteria = true;

		const int impliedNumberOfResolutions = criterion.second.size();

		if (this->m_NumberOfLevelsLastSetBy == "") // check if some other settings set the NumberOfLevels
		{
			// try catch?
			this->m_SyNImageRegistrationMethod->SetNumberOfLevels(impliedNumberOfResolutions);
			this->m_NumberOfLevelsLastSetBy = criterion.first;
		}
		else
		{
			if (this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != impliedNumberOfResolutions)
			{
				// TODO log error?
				std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
				meetsCriteria = false;
				return meetsCriteria;
			}
		}

		itk::Array< InternalComputationValueType > smoothingSigmasPerLevel;

		smoothingSigmasPerLevel.SetSize(impliedNumberOfResolutions);

		unsigned int resolutionIndex = 0;
		for (auto const & criterionValue : criterion.second) // auto&& preferred?
		{
			smoothingSigmasPerLevel[resolutionIndex] = std::stoi(criterionValue);
			++resolutionIndex;
		}
		// try catch?
		// Smooth by specified gaussian sigmas for each level.  These values are specified in
		// physical units.
		this->m_SyNImageRegistrationMethod->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	} else if( criterion.first == "LearningRate" ) {
		meetsCriteria = true;
		this->m_SyNImageRegistrationMethod->SetLearningRate(std::stof(criterion.second[0]));
	} else if( criterion.first == "NumberOfIterations" ) {
			meetsCriteria = true;

      if(this->m_NumberOfLevelsLastSetBy != "" &&
         this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != criterion.second.size()) {
         std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
         return false;
      }

      typename SyNImageRegistrationMethodType::NumberOfIterationsArrayType numberOfIterations(criterion.second.size());
      for(int i = 0; i < criterion.second.size(); i++) {
        numberOfIterations[i] = stoull(criterion.second[i]);
        std::cout << "numberOfIterations[" << i << "]:" << numberOfIterations[i] << std::endl;
      }
      this->m_SyNImageRegistrationMethod->SetNumberOfIterationsPerLevel(numberOfIterations);
	}

	return meetsCriteria;
}
} //end namespace selx
