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

#include "selxItkImageRegistrationMethodv4Component.h"

//TODO: get rid of these
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageFileWriter.h"

namespace selx
{
template< typename TFilter >
class CommandIterationUpdate : public itk::Command
{
public:

  typedef CommandIterationUpdate    Self;
  typedef itk::Command              Superclass;
  typedef itk::SmartPointer< Self > Pointer;
  itkNewMacro( Self );

  typedef itk::GradientDescentOptimizerv4 OptimizerType;
  typedef   const OptimizerType *         OptimizerPointer;

protected:

  CommandIterationUpdate() {}

public:

  virtual void Execute( itk::Object * caller, const itk::EventObject & event ) ITK_OVERRIDE
  {
    Execute( (const itk::Object *)caller, event );
  }


  virtual void Execute( const itk::Object * object, const itk::EventObject & event ) ITK_OVERRIDE
  {
    const TFilter * filter = static_cast< const TFilter * >( object );
    if( typeid( event ) == typeid( itk::MultiResolutionIterationEvent ) )
    {
      unsigned int currentLevel = filter->GetCurrentLevel();
      typename TFilter::ShrinkFactorsPerDimensionContainerType shrinkFactors = filter->GetShrinkFactorsPerDimension( currentLevel );
      typename TFilter::SmoothingSigmasArrayType smoothingSigmas             = filter->GetSmoothingSigmasPerLevel();
      typename TFilter::TransformParametersAdaptorsContainerType adaptors    = filter->GetTransformParametersAdaptorsPerLevel();

      const itk::ObjectToObjectOptimizerBase * optimizerBase = filter->GetOptimizer();
      typedef itk::GradientDescentOptimizerv4 GradientDescentOptimizerv4Type;
      typename GradientDescentOptimizerv4Type::ConstPointer optimizer = dynamic_cast< const GradientDescentOptimizerv4Type * >( optimizerBase );
      if( !optimizer )
      {
        itkGenericExceptionMacro( "Error dynamic_cast failed" );
      }
      typename GradientDescentOptimizerv4Type::DerivativeType gradient = optimizer->GetGradient();

      /* orig
      std::cout << "  Current level = " << currentLevel << std::endl;
      std::cout << "    shrink factor = " << shrinkFactors[currentLevel] << std::endl;
      std::cout << "    smoothing sigma = " << smoothingSigmas[currentLevel] << std::endl;
      std::cout << "    required fixed parameters = " << adaptors[currentLevel]->GetRequiredFixedParameters() << std::endl;
      */

      //debug:
      std::cout << "  CL Current level:           " << currentLevel << std::endl;
      std::cout << "   SF Shrink factor:          " << shrinkFactors << std::endl;
      std::cout << "   SS Smoothing sigma:        " << smoothingSigmas[ currentLevel ] << std::endl;
      //std::cout << "   RFP Required fixed params: " << adaptors[ currentLevel ]->GetRequiredFixedParameters() << std::endl;
      std::cout << "   LR Final learning rate:    " << optimizer->GetLearningRate() << std::endl;
      std::cout << "   FM Final metric value:     " << optimizer->GetCurrentMetricValue() << std::endl;
      std::cout << "   SC Optimizer scales:       " << optimizer->GetScales() << std::endl;
      std::cout << "   FG Final metric gradient (sample of values): ";
      if( gradient.GetSize() < 16 )
      {
        std::cout << gradient;
      }
      else
      {
        for( itk::SizeValueType i = 0; i < gradient.GetSize(); i += ( gradient.GetSize() / 16 ) )
        {
          std::cout << gradient[ i ] << " ";
        }
      }
      std::cout << std::endl;
    }
    else if( !( itk::IterationEvent().CheckEvent( &event ) ) )
    {
      return;
    }
    else
    {
      OptimizerPointer optimizer = static_cast< OptimizerPointer >( object );
      std::cout << optimizer->GetCurrentIteration() << ": ";
      std::cout << optimizer->GetCurrentMetricValue() << std::endl;
      //std::cout << optimizer->GetInfinityNormOfProjectedGradient() << std::endl;
    }
  }
};

template< int Dimensionality, class TPixel >
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::ItkImageRegistrationMethodv4Component() : m_TransformAdaptorsContainerInterface(
    nullptr )
{
  m_theItkFilter = TheItkFilterType::New();
  m_theItkFilter->InPlaceOn();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel >
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::~ItkImageRegistrationMethodv4Component()
{
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >
::Set( itkImageFixedInterface< Dimensionality, TPixel > * component )
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage( fixedImage );

  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >
::Set( itkImageMovingInterface< Dimensionality, TPixel > * component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage( movingImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::Set( itkTransformInterface< TransformInternalComputationValueType,
  Dimensionality > * component )
{
  this->m_theItkFilter->SetInitialTransform( component->GetItkTransform() );

  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::Set( TransformParametersAdaptorsContainerInterfaceType * component )
{
  // store the interface to the ParametersAdaptorsContainer since during the setup of the connections the TransformParametersAdaptorComponent might not be fully connected and thus does not have the adaptors ready.
  this->m_TransformAdaptorsContainerInterface = component;
  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::Set( itkMetricv4Interface< Dimensionality, TPixel > * component )
{
  //TODO: The optimizer must be set explicitly, since this is a work-around for a bug in itkRegistrationMethodv4.
  //TODO: report bug to itk: when setting a metric, the optimizer must be set explicitly as well, since default optimizer setup breaks.
  this->m_theItkFilter->SetMetric( component->GetItkMetricv4() );

  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::Set( itkOptimizerv4Interface< OptimizerInternalComputationValueType > * component )
{
  //TODO: The optimizer must be set explicitly, since this is a work-around for a bug in itkRegistrationMethodv4.
  //TODO: report bug to itk: when setting a metric, the optimizer must be set explicitly as well, since default optimizer setup breaks.
  this->m_theItkFilter->SetOptimizer( component->GetItkOptimizerv4() );

  return 0;
}


template< int Dimensionality, class TPixel >
void
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::RunRegistration( void )
{
  typename FixedImageType::ConstPointer fixedImage   = this->m_theItkFilter->GetFixedImage();
  typename MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  // Scale estimator is not used in current implementation yet
  typename ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();

  ImageMetricType * theMetric = dynamic_cast< ImageMetricType * >( this->m_theItkFilter->GetModifiableMetric() );

  auto optimizer = dynamic_cast< itk::GradientDescentOptimizerv4 * >( this->m_theItkFilter->GetModifiableOptimizer() );
  //auto optimizer = dynamic_cast<itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > *>(this->m_theItkFilter->GetModifiableOptimizer());

  auto transform = this->m_theItkFilter->GetModifiableTransform();

  if( theMetric )
  {
    scalesEstimator->SetMetric( theMetric );
  }
  else
  {
    itkExceptionMacro( "Error casting to ImageMetricv4Type failed" );
  }

  //std::cout << "estimated step scale: " << scalesEstimator->EstimateStepScale(1.0);
  scalesEstimator->SetTransformForward( true );
  scalesEstimator->SetSmallParameterVariation( 1.0 );

  optimizer->SetScalesEstimator( ITK_NULLPTR );
  //optimizer->SetScalesEstimator(scalesEstimator);
  optimizer->SetDoEstimateLearningRateOnce( false ); //true by default
  optimizer->SetDoEstimateLearningRateAtEachIteration( false );

  this->m_theItkFilter->SetOptimizer( optimizer );

  if( this->m_TransformAdaptorsContainerInterface != nullptr )
  {
    auto adaptors = this->m_TransformAdaptorsContainerInterface->GetItkTransformParametersAdaptorsContainer();
    this->m_theItkFilter->SetTransformParametersAdaptorsPerLevel( adaptors 
        );
  }

  typedef CommandIterationUpdate< TheItkFilterType > RegistrationCommandType;
  typename RegistrationCommandType::Pointer registrationObserver = RegistrationCommandType::New();
  this->m_theItkFilter->AddObserver( itk::IterationEvent(), registrationObserver );

  // perform the actual registration
  this->m_theItkFilter->Update();
}


template< int Dimensionality, class TPixel >
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >::TransformPointer
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >
::GetItkTransform()
{
  return this->m_theItkFilter->GetModifiableTransform();
}


template< int Dimensionality, class TPixel >
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  if( criterion.first == "ComponentProperty" )
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != "SomeProperty" )  // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "Dimensionality" ) //Supports this?
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( std::stoi( criterionValue ) != Dimensionality )
      {
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "PixelType" ) //Supports this?
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != Self::GetPixelTypeNameString() )
      {
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "NumberOfLevels" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
      {
        // try catch?
        this->m_theItkFilter->SetNumberOfLevels( std::stoi( criterion.second[ 0 ] ) );
        this->m_NumberOfLevelsLastSetBy = criterion.first;
      }
      else
      {
        if( this->m_theItkFilter->GetNumberOfLevels() != std::stoi( criterion.second[ 0 ] ) )
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
  else if( criterion.first == "ShrinkFactorsPerLevel" ) //Supports this?
  {
    meetsCriteria = true;

    const int impliedNumberOfResolutions = criterion.second.size();

    if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
    {
      // try catch?
      this->m_theItkFilter->SetNumberOfLevels( impliedNumberOfResolutions );
      this->m_NumberOfLevelsLastSetBy = criterion.first;
    }
    else
    {
      if( this->m_theItkFilter->GetNumberOfLevels() != impliedNumberOfResolutions )
      {
        // TODO log error?
        std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
        meetsCriteria = false;
        return meetsCriteria;
      }
    }

    itk::Array< itk::SizeValueType > shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( impliedNumberOfResolutions );

    unsigned int resolutionIndex = 0;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      shrinkFactorsPerLevel[ resolutionIndex ] = std::stoi( criterionValue );
      ++resolutionIndex;
    }
    // try catch?
    this->m_theItkFilter->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
  }
  else if( criterion.first == "SmoothingSigmasPerLevel" ) //Supports this?
  {
    meetsCriteria = true;

    const int impliedNumberOfResolutions = criterion.second.size();

    if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
    {
      // try catch?
      this->m_theItkFilter->SetNumberOfLevels( impliedNumberOfResolutions );
      this->m_NumberOfLevelsLastSetBy = criterion.first;
    }
    else
    {
      if( this->m_theItkFilter->GetNumberOfLevels() != impliedNumberOfResolutions )
      {
        // TODO log error?
        std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
        meetsCriteria = false;
        return meetsCriteria;
      }
    }

    itk::Array< TransformInternalComputationValueType > smoothingSigmasPerLevel;

    smoothingSigmasPerLevel.SetSize( impliedNumberOfResolutions );

    unsigned int resolutionIndex = 0;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      smoothingSigmasPerLevel[ resolutionIndex ] = std::stoi( criterionValue );
      ++resolutionIndex;
    }
    // try catch?
    // Smooth by specified gaussian sigmas for each level.  These values are specified in
    // physical units.
    this->m_theItkFilter->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
  }

  return meetsCriteria;
}
} //end namespace selx
