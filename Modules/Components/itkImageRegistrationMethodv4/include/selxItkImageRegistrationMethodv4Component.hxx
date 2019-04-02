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
#include "selxCheckTemplateProperties.h"
#include "selxStringConverter.h"

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

      // TODO optimizer is can be ObjectToObjectOptimizerBaseTemplate<double> or ObjectToObjectOptimizerBaseTemplate<float>
      // dynamic cast will fail on <float>, since GradientDescentOptimizerv4Type is by default <double>
      auto * optimizerBase = filter->GetOptimizer();
      typedef itk::GradientDescentOptimizerv4 GradientDescentOptimizerv4Type;
      typename GradientDescentOptimizerv4Type::ConstPointer optimizer = dynamic_cast< const GradientDescentOptimizerv4Type * >( optimizerBase );
      if( !optimizer )
      {
        itkGenericExceptionMacro( "Error dynamic_cast failed" );
      }
      typename GradientDescentOptimizerv4Type::DerivativeType gradient = optimizer->GetGradient();

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
        std::cout << std::setprecision(6) << gradient;
      }
      else
      {
        for( itk::SizeValueType i = 0; i < gradient.GetSize(); i += ( gradient.GetSize() / 16 ) )
        {
          std::cout << std::setprecision(6) << gradient[ i ] << " ";
        }
      }
      std::cout << std::endl;
    }
    else if( typeid( event ) == typeid( itk::IterationEvent ) )
    {
      typedef itk::GradientDescentOptimizerv4 GradientDescentOptimizerv4Type;
      auto * optimizerBase = filter->GetOptimizer();
      typename GradientDescentOptimizerv4Type::ConstPointer optimizer = dynamic_cast< const GradientDescentOptimizerv4Type * >( optimizerBase );
      if( !optimizer )
      {
        itkGenericExceptionMacro( "Error dynamic_cast failed" );
      }
      std::cout << "   Iteration #:    " << optimizer->GetCurrentIteration() << std::endl;
      std::cout << "   LR Final learning rate:    " << optimizer->GetLearningRate() << std::endl;
      std::cout << "   FM Final metric value:     " << optimizer->GetCurrentMetricValue() << std::endl;
    }
    else if( !( itk::IterationEvent().CheckEvent( &event ) ) )
    {
      return;
    }
  }
};

template< int Dimensionality, class TPixel, class InternalComputationValueType >
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel,
InternalComputationValueType >::ItkImageRegistrationMethodv4Component( const std::string & name, LoggerImpl & logger )
  : Superclass( name, logger ), m_TransformAdaptorsContainerInterface( nullptr ), m_InvertIntensity(false), m_MetricSamplingPercentage(1.0)
{
  this->m_ImageRegistrationMethodv4Filter = ImageRegistrationMethodv4Type::New();
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::~ItkImageRegistrationMethodv4Component()
{
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_FixedImage = component->GetItkImageFixed();
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_MovingImage = component->GetItkImageMoving();
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::Accept( typename
  itkTransformInterface< InternalComputationValueType,
  Dimensionality >::Pointer component )
{
  this->m_Transform = component->GetItkTransform();
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::Accept(
  typename TransformParametersAdaptorsContainerInterfaceType::Pointer component )
{
  this->m_TransformAdaptorsContainerInterface = component;
  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::Accept( typename itkMetricv4Interface< Dimensionality,
  TPixel,
  InternalComputationValueType >::Pointer component )
{
  //TODO: The optimizer must be set explicitly, since this is a work-around for a bug in itkRegistrationMethodv4.
  //TODO: report bug to itk: when setting a metric, the optimizer must be set explicitly as well, since default optimizer setup breaks.
  this->m_ImageRegistrationMethodv4Filter->SetMetric( component->GetItkMetricv4() );

  return 0;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
int
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::Accept( typename
  itkOptimizerv4Interface< InternalComputationValueType >::Pointer component )
{
  //TODO: The optimizer must be set explicitly, since this is a work-around for a bug in itkRegistrationMethodv4.
  //TODO: report bug to itk: when setting a metric, the optimizer must be set explicitly as well, since default optimizer setup breaks.
  this->m_ImageRegistrationMethodv4Filter->SetOptimizer( component->GetItkOptimizerv4() );

  return 0;
}

template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::BeforeUpdate( void ) {
  FixedImagePointer fixedImage = this->m_FixedImage;
  MovingImagePointer movingImage = this->m_MovingImage;

  fixedImage->Update();
  movingImage->Update();

  if (this->m_RescaleIntensity.size() == 2) {
    this->m_Logger.Log(LogLevel::INF, "{0}: Rescaling images to [{1}, {2}]", this->m_Name, this->m_RescaleIntensity[0], this->m_RescaleIntensity[1]);

    FixedRescaleImageFilterPointer fixedIntensityRescaler = FixedRescaleImageFilterType::New();
    fixedIntensityRescaler->SetInput(fixedImage);
    fixedIntensityRescaler->SetOutputMinimum(std::stof(this->m_RescaleIntensity[0]));
    fixedIntensityRescaler->SetOutputMaximum(std::stof(this->m_RescaleIntensity[1]));
    fixedIntensityRescaler->UpdateOutputInformation();
    fixedImage = fixedIntensityRescaler->GetOutput();
    fixedImage->Update();

    MovingRescaleImageFilterPointer movingIntensityRescaler = MovingRescaleImageFilterType::New();
    movingIntensityRescaler->SetInput(movingImage);
    movingIntensityRescaler->SetOutputMinimum(std::stof(this->m_RescaleIntensity[0]));
    movingIntensityRescaler->SetOutputMaximum(std::stof(this->m_RescaleIntensity[1]));
    movingIntensityRescaler->UpdateOutputInformation();
    movingImage = movingIntensityRescaler->GetOutput();
    movingImage->Update();
  }

  if (this->m_InvertIntensity) {
    this->m_Logger.Log(LogLevel::INF, "{0}, Inverting image scales", this->m_Name);

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

  this->m_ImageRegistrationMethodv4Filter->SetFixedImage(fixedImage);
  this->m_ImageRegistrationMethodv4Filter->SetMovingImage(movingImage);

  if(this->m_MetricSamplingPercentage < 1.0) {
    this->m_ImageRegistrationMethodv4Filter->SetMetricSamplingPercentage(this->m_MetricSamplingPercentage);
  }
}

template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::Update( void )
{
  // Set transform
  typedef itk::CompositeTransform<InternalComputationValueType, Dimensionality >  MovingCompositeTransformType;

  this->m_ImageRegistrationMethodv4Filter->SetInitialTransform(this->m_Transform);

  // Configure scales estimator
  typename ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetTransformForward( true );
  scalesEstimator->SetSmallParameterVariation( 1.0 );

  ImageMetricType * metric = dynamic_cast< ImageMetricType * >( this->m_ImageRegistrationMethodv4Filter->GetModifiableMetric() );
  if( metric )
  {
    scalesEstimator->SetMetric( metric );
  }
  else
  {
    throw std::runtime_error( "Error casting to ImageMetricv4Type failed" );
  }

  auto optimizer = this->m_ImageRegistrationMethodv4Filter->GetModifiableOptimizer();
  optimizer->SetScalesEstimator(scalesEstimator);

  // Multi-resolution setup
  if( this->m_TransformAdaptorsContainerInterface != nullptr )
  {
    auto adaptors = this->m_TransformAdaptorsContainerInterface->GetItkTransformParametersAdaptorsContainer();
    this->m_ImageRegistrationMethodv4Filter->SetTransformParametersAdaptorsPerLevel(adaptors);
  }

  typedef CommandIterationUpdate< ImageRegistrationMethodv4Type > RegistrationCommandType;
  typename RegistrationCommandType::Pointer registrationObserver = RegistrationCommandType::New();
  this->m_ImageRegistrationMethodv4Filter->AddObserver( itk::IterationEvent(), registrationObserver );

  // Perform the actual registration
  this->m_ImageRegistrationMethodv4Filter->Update();
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >::TransformPointer
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::GetItkTransform()
{
  return this->m_Transform;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::SetFixedInitialTransform( typename CompositeTransformType::Pointer fixedInitialTransform )
{
  return this->m_ImageRegistrationMethodv4Filter->SetFixedInitialTransform( fixedInitialTransform );
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
void
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::SetMovingInitialTransform( typename CompositeTransformType::Pointer movingInitialTransform )
{
  return this->m_ImageRegistrationMethodv4Filter->SetMovingInitialTransform( movingInitialTransform );
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
  if( criterion.first == "NumberOfLevels" ) //Supports this?
  {
    if( hasOneCriterionValue )
    {
      if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
      {
        this->m_ImageRegistrationMethodv4Filter->SetNumberOfLevels( std::stoi( criterion.second[ 0 ] ) );
        this->m_NumberOfLevelsLastSetBy = criterion.first;
        return true;
      }
      else
      {
        if( this->m_ImageRegistrationMethodv4Filter->GetNumberOfLevels() != std::stoi( criterion.second[ 0 ] ) )
        {
          // TODO log error?
          std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
          return false;
        }
      }
    }
    else
    {
      return false;
    }
  }
  else if( criterion.first == "ShrinkFactorsPerLevel" ) //Supports this?
  {
    const int impliedNumberOfResolutions = criterion.second.size();

    if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
    {
      this->m_ImageRegistrationMethodv4Filter->SetNumberOfLevels( impliedNumberOfResolutions );
      this->m_NumberOfLevelsLastSetBy = criterion.first;
      return true;
    }
    else
    {
      if( this->m_ImageRegistrationMethodv4Filter->GetNumberOfLevels() != impliedNumberOfResolutions )
      {
        // TODO log error?
        std::cout << "A conflicting NumberOfLevels was set by " << this->m_NumberOfLevelsLastSetBy << std::endl;
        return false;
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
    this->m_ImageRegistrationMethodv4Filter->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
    return true;
  }
  else if( criterion.first == "SmoothingSigmasPerLevel" ) //Supports this?
  {
    const int impliedNumberOfResolutions = criterion.second.size();

    if( this->m_NumberOfLevelsLastSetBy == "" ) // check if some other settings set the NumberOfLevels
    {
      this->m_ImageRegistrationMethodv4Filter->SetNumberOfLevels( impliedNumberOfResolutions );
      this->m_NumberOfLevelsLastSetBy = criterion.first;
      return true;
    }
    else
    {
      if( this->m_ImageRegistrationMethodv4Filter->GetNumberOfLevels() != impliedNumberOfResolutions )
      {
        this->m_Logger.Log(LogLevel::ERR, "A conflicting NumberOfLevels was set by {0}.", this->m_NumberOfLevelsLastSetBy);
        return false;
      }
    }

    itk::Array< InternalComputationValueType > smoothingSigmasPerLevel;

    smoothingSigmasPerLevel.SetSize( impliedNumberOfResolutions );

    unsigned int resolutionIndex = 0;
    for( const auto& criterionValue : criterion.second )
    {
      smoothingSigmasPerLevel[ resolutionIndex ] = std::stoi( criterionValue );
      ++resolutionIndex;
    }
    this->m_ImageRegistrationMethodv4Filter->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    return true;
  } else if( criterion.first == "RescaleIntensity" ) {
    if( criterion.second.size() == 2 ) {
      this->m_RescaleIntensity = criterion.second;
      return true;
    }
    else {
      this->m_Logger.Log(LogLevel::ERR, "Expected two values for RescaleIntensity (min, max), got {0}.", criterion.second.size());
      return false;
    }
  } else if( criterion.first == "InvertIntensity" ) {
    if( hasOneCriterionValue ) {
      bool ok = StringConverter::Convert(criterion.second[0], this->m_InvertIntensity);
      if (ok) {
        return true;
      } else {
        this->m_Logger.Log(LogLevel::ERR, "Expected InvertIntensity to be True or False, got {0}.", criterion.second[0]);
      }
    } else {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for InvertIntensity (True or False), got {0}.", criterion.second.size());
    }
  } else if( criterion.first == "MetricSamplingPercentage" ) {
    if( hasOneCriterionValue ) {
      this->m_MetricSamplingPercentage = std::stof(criterion.second[0]);
      return true;
    } else {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for MetricSamplingPercetage, got {0}.", criterion.second.size());
    }
  } else if( criterion.first == "MetricSamplingStrategy" ) {
    if( hasOneCriterionValue ) {
      if( criterion.second[0] == "Regular" ) {
        this->m_ImageRegistrationMethodv4Filter->SetMetricSamplingStrategy(ImageRegistrationMethodv4Type::MetricSamplingStrategyType::REGULAR);
        return true;
      } else if( criterion.second[0] == "Random" ) {
        this->m_ImageRegistrationMethodv4Filter->SetMetricSamplingStrategy(ImageRegistrationMethodv4Type::MetricSamplingStrategyType::RANDOM);
        return true;
      } else {
        this->m_Logger.Log(LogLevel::ERR, "Expected MetricSamplingStrategy to be Regular or Random, got {0}.", criterion.second[0]);
      }
    } else {
      this->m_Logger.Log(LogLevel::ERR, "Expected one value for MetricSamplingStrategy (Regular or Random), got {0}.", criterion.second.size());
    }
  }

  return false;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel, InternalComputationValueType >
::ConnectionsSatisfied()
{
  // This function overrides the default behavior, in which all accepting interfaces must be set, by allowing the itkTransformParametersAdaptorsContainerInterface not being set.
  // TODO: see I we can reduce the amount of code with helper (meta-)functions
  if( !this->InterfaceAcceptor< itkImageFixedInterface< Dimensionality, TPixel >>::GetAccepted())
  {
    return false;
  }
  if( !this->InterfaceAcceptor< itkImageMovingInterface< Dimensionality, TPixel >>::GetAccepted() )
  {
    return false;
  }
  if( !this->InterfaceAcceptor< itkTransformInterface< InternalComputationValueType, Dimensionality >>::GetAccepted() )
  {
    return false;
  }
  // Allow unconnected itkTransformParametersAdaptorsContainerInterface (not needed for affine transform)
  // itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >
  if( !this->InterfaceAcceptor< itkMetricv4Interface< Dimensionality, TPixel, InternalComputationValueType >>::GetAccepted() )
  {
    return false;
  }
  if( !this->InterfaceAcceptor< itkOptimizerv4Interface< InternalComputationValueType >>::GetAccepted() )
  {
    return false;
  }
  return true;
}
} //end namespace selx
