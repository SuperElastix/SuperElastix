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
#include "selxStringConverter.h"

namespace selx
{
template< int Dimensionality, class TPixel , class InternalComputationValueType >
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >
  ::ItkSyNImageRegistrationMethodComponent( const std::string & name, LoggerImpl & logger ) 
  : Superclass( name, logger ), m_InvertIntensity(false), m_MetricSamplingPercentage(1.0)
{
  this->m_SyNImageRegistrationMethod = SyNImageRegistrationMethodType::New();

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
	FixedImagePointer fixedImage = this->m_FixedImage;
	MovingImagePointer movingImage = this->m_MovingImage;

  fixedImage->Update();
  movingImage->Update();

	if (this->m_RescaleIntensity.size() == 2) {
		this->m_Logger.Log(LogLevel::INF, "{0}: Rescaling images to [{1}, {2}].", this->m_Name, this->m_RescaleIntensity[0], this->m_RescaleIntensity[1]);

		FixedRescaleImageFilterPointer fixedIntensityRescaler = FixedRescaleImageFilterType::New();
		fixedIntensityRescaler->SetInput(fixedImage);
		fixedIntensityRescaler->SetOutputMinimum(StringConverter{ this->m_RescaleIntensity[0] });
		fixedIntensityRescaler->SetOutputMaximum(StringConverter{ this->m_RescaleIntensity[1] });
		fixedIntensityRescaler->UpdateOutputInformation();
		fixedImage = fixedIntensityRescaler->GetOutput();
		fixedImage->Update();

		MovingRescaleImageFilterPointer movingIntensityRescaler = MovingRescaleImageFilterType::New();
		movingIntensityRescaler->SetInput(movingImage);
		movingIntensityRescaler->SetOutputMinimum(StringConverter{ this->m_RescaleIntensity[0] });
		movingIntensityRescaler->SetOutputMaximum(StringConverter{ this->m_RescaleIntensity[1] });
		movingIntensityRescaler->UpdateOutputInformation();
		movingImage = movingIntensityRescaler->GetOutput();
		movingImage->Update();
	}

	if (this->m_InvertIntensity) {
		this->m_Logger.Log(LogLevel::INF, "{0}: Inverting image scales", this->m_Name);

		FixedImageCalculatorFilterPointer fixedIntensityMaximumCalculator = FixedImageCalculatorFilterType::New();
		fixedIntensityMaximumCalculator->SetImage(fixedImage);
		fixedIntensityMaximumCalculator->ComputeMaximum();

		FixedInvertIntensityImageFilterPointer fixedIntensityInverter = FixedInvertIntensityImageFilterType::New();
		fixedIntensityInverter->SetInput(fixedImage);
		fixedIntensityInverter->SetMaximum(fixedIntensityMaximumCalculator->GetMaximum());
		fixedIntensityInverter->UpdateOutputInformation();
		fixedImage = fixedIntensityInverter->GetOutput();
		fixedImage->Update();

		MovingImageCalculatorFilterPointer movingIntensityMaximumCalculator = MovingImageCalculatorFilterType::New();
		movingIntensityMaximumCalculator->SetImage(movingImage);
		movingIntensityMaximumCalculator->ComputeMaximum();

		MovingInvertIntensityImageFilterPointer movingIntensityInverter = MovingInvertIntensityImageFilterType::New();
		movingIntensityInverter->SetInput(movingImage);
		movingIntensityInverter->SetMaximum(movingIntensityMaximumCalculator->GetMaximum());
		movingIntensityInverter->UpdateOutputInformation();
		movingImage = movingIntensityInverter->GetOutput();
		movingImage->Update();
	}

	this->m_SyNImageRegistrationMethod->SetFixedImage(fixedImage);
	this->m_SyNImageRegistrationMethod->SetMovingImage(movingImage);

	if(this->m_MetricSamplingPercentage < 1.0) {
		this->m_SyNImageRegistrationMethod->SetMetricSamplingPercentage(this->m_MetricSamplingPercentage);
	}
};

template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel, InternalComputationValueType >::Update( void )
{

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
::MeetsCriterion(const ComponentBase::CriterionType & criterion)
{
  const auto& criterionKey = criterion.first;
  const auto& criterionValues = criterion.second;
  const bool hasOneCriterionValue = criterionValues.size() == 1;

  // First check if user-provided properties are template properties and if this component was instantiated with those template properties.
  switch (CheckTemplateProperties(this->TemplateProperties(), criterion))
  {
    case CriterionStatus::Satisfied:
    {
      return true;
    }
    case CriterionStatus::Failed:
    {
      return false;
    }
    case CriterionStatus::Unknown:
    {
      // Just continue.
    }
  }

  // Next else-if blocks check if the name of setting is an existing property for this component, otherwise MeetsCriterion returns CriterionStatus::Failed.
  if (criterionKey == "NumberOfLevels") //Does the Components have this setting?
  {
    if (hasOneCriterionValue)
    {
      if (this->m_NumberOfLevelsLastSetBy.empty()) // check if some other settings set the NumberOfLevels
      {
        // try catch?
        this->m_SyNImageRegistrationMethod->SetNumberOfLevels(StringConverter{ criterionValues[0] });
        this->m_NumberOfLevelsLastSetBy = criterionKey;
      }
      else
      {
        if (!StringConverter::IsStringConvertibleToValue(
          criterionValues[0],
          this->m_SyNImageRegistrationMethod->GetNumberOfLevels()))
        {
          // TODO log error?
          std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
          return false;
        }
      }
      return true;
    }
    else
    {
      // TODO log error?
      std::cout << "NumberOfLevels accepts one number only" << std::endl;
      return false;
    }
  }
  else if (criterionKey == "ShrinkFactorsPerLevel") //Supports this?
  {
    const int impliedNumberOfResolutions = criterionValues.size();

    if (this->m_NumberOfLevelsLastSetBy.empty()) // check if some other settings set the NumberOfLevels
    {
      // try catch?
      this->m_SyNImageRegistrationMethod->SetNumberOfLevels(impliedNumberOfResolutions);
      this->m_NumberOfLevelsLastSetBy = criterionKey;
    }
    else
    {
      if (this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != impliedNumberOfResolutions)
      {
        // TODO log error?
        std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
        return false;
      }
    }

    itk::Array< itk::SizeValueType > shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize(impliedNumberOfResolutions);

    unsigned int resolutionIndex = 0;
    for (auto const & criterionValue : criterionValues)
    {
      shrinkFactorsPerLevel[resolutionIndex] = StringConverter{ criterionValue };
      ++resolutionIndex;
    }
    // try catch?
    this->m_SyNImageRegistrationMethod->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
    return true;
  }
  else if (criterionKey == "SmoothingSigmasPerLevel") //Supports this?
  {
    const int impliedNumberOfResolutions = criterionValues.size();

    if (this->m_NumberOfLevelsLastSetBy.empty()) // check if some other settings set the NumberOfLevels
    {
      // try catch?
      this->m_SyNImageRegistrationMethod->SetNumberOfLevels(impliedNumberOfResolutions);
      this->m_NumberOfLevelsLastSetBy = criterionKey;
    }
    else
    {
      if (this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != impliedNumberOfResolutions)
      {
        // TODO log error?
        std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
        return false;
      }
    }

    itk::Array< InternalComputationValueType > smoothingSigmasPerLevel;

    smoothingSigmasPerLevel.SetSize(impliedNumberOfResolutions);

    unsigned int resolutionIndex = 0;
    for (auto const & criterionValue : criterionValues)
    {
      smoothingSigmasPerLevel[resolutionIndex] = StringConverter{ criterionValue };
      ++resolutionIndex;
    }
    // try catch?
    // Smooth by specified gaussian sigmas for each level.  These values are specified in
    // physical units.
    this->m_SyNImageRegistrationMethod->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
    return true;
  }
  else if (criterionKey == "LearningRate") {
    this->m_SyNImageRegistrationMethod->SetLearningRate(StringConverter{ criterionValues[0] });
    return true;
  }
  else if (hasOneCriterionValue && (criterionKey == "ConvergenceThreshold"))
  {
    this->m_SyNImageRegistrationMethod->SetConvergenceThreshold(StringConverter{ criterionValues[0] });
    return true;
  }
  else if (hasOneCriterionValue && (criterionKey == "ConvergenceWindowSize"))
  {
    this->m_SyNImageRegistrationMethod->SetConvergenceWindowSize(StringConverter{ criterionValues[0] });
    return true;
  }
  else if (hasOneCriterionValue && (criterionKey == "GaussianSmoothingVarianceForTheUpdateField"))
  {
    this->m_SyNImageRegistrationMethod->SetGaussianSmoothingVarianceForTheUpdateField(StringConverter{ criterionValues[0] });
    return true;
  }
  else if (hasOneCriterionValue && (criterionKey == "GaussianSmoothingVarianceForTheTotalField"))
  {
    this->m_SyNImageRegistrationMethod->SetGaussianSmoothingVarianceForTheTotalField(StringConverter{ criterionValues[0] });
    return true;
  }
  else if (criterionKey == "NumberOfIterations") {
    if (this->m_NumberOfLevelsLastSetBy != "" &&
      this->m_SyNImageRegistrationMethod->GetNumberOfLevels() != criterionValues.size()) {
      std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
      return false;
    }

    typename SyNImageRegistrationMethodType::NumberOfIterationsArrayType numberOfIterations(criterionValues.size());
    for (int i = 0; i < criterionValues.size(); i++) {
      numberOfIterations[i] = StringConverter{ criterionValues[i] };
    }
    this->m_SyNImageRegistrationMethod->SetNumberOfIterationsPerLevel(numberOfIterations);
    return true;
  }
  else if (criterionKey == "EstimateScales") //Supports this?
  {
    if (criterionValues.size() != 1)
    {
      return false;
    }
    else
    {
      bool criterionValue = false;

      if (StringConverter::Convert(*criterionValues.begin(), criterionValue))
      {
        auto optimizer = this->m_SyNImageRegistrationMethod->GetModifiableOptimizer();
        optimizer->SetDoEstimateScales(criterionValue);
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  else if (criterionKey == "RescaleIntensity") {
    if (criterionValues.size() != 2) {
      this->m_Logger.Log(LogLevel::ERR, "Expected two values for RescaleIntensity (min, max), got {0}.", criterionValues.size());
      return false;
    }
    else {
      this->m_RescaleIntensity = criterionValues;
      return true;
    }
  }
  else if (criterionKey == "InvertIntensity") {
    if (criterionValues.size() != 1) {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for InvertIntensity (True or False), got {0}.", criterionValues.size());
      return false;
    }
    else {
      if (StringConverter::Convert(criterionValues[0], this->m_InvertIntensity))
      {
        return true;
      }
      else
      {
        this->m_Logger.Log(LogLevel::ERR, "Expected InvertIntensity to be True or False, got {0}.", criterionValues[0]);
        return false;
      }
    }
  }
  else if (criterionKey == "MetricSamplingPercentage") {
    if (!hasOneCriterionValue) {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for MetricSamplingPercetage, got {0}.", criterionValues.size());
      return false;
    }
    else {
      this->m_MetricSamplingPercentage = StringConverter{ criterionValues[0] };
      return true;
    }
  }
  else if (criterionKey == "MetricSamplingStrategy") {
    if (!hasOneCriterionValue) {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for MetricSamplingStrategy (Regular or Random), got {0}.", criterionValues.size());
      return false;
    }
    else {
      if (criterionValues[0] == "Regular") {
        this->m_SyNImageRegistrationMethod->SetMetricSamplingStrategy(SyNImageRegistrationMethodType::MetricSamplingStrategyType::REGULAR);
        return true;
      }
      else if (criterionValues[0] == "Random") {
        this->m_SyNImageRegistrationMethod->SetMetricSamplingStrategy(SyNImageRegistrationMethodType::MetricSamplingStrategyType::RANDOM);
        return true;
      }
      else {
        this->m_Logger.Log(LogLevel::ERR, "Expected MetricSamplingStrategy to be Regular or Random, got {0}.", criterionValues[0]);
        return false;
      }
    }
  }
  else
  {
    return false;
  }
}
} //end namespace selx
