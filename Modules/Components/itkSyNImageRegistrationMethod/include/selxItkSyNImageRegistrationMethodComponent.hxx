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
template< int Dimensionality, class TPixel >
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >::ItkSyNImageRegistrationMethodComponent()
{
  m_theItkFilter = TheItkFilterType::New();
  m_theItkFilter->InPlaceOn();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel >
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >::~ItkSyNImageRegistrationMethodComponent()
{
}


template< int Dimensionality, class TPixel >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >
::Set( itkImageFixedInterface< Dimensionality, TPixel > * component )
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage( fixedImage );

  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >
::Set( itkImageMovingInterface< Dimensionality, TPixel > * component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage( movingImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >::Set( itkMetricv4Interface< Dimensionality, TPixel, double > * component )
{
  this->m_theItkFilter->SetMetric( component->GetItkMetricv4() );

  return 0;
}


template< int Dimensionality, class TPixel >
void
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >::RunRegistration( void )
{
  typename FixedImageType::ConstPointer fixedImage   = this->m_theItkFilter->GetFixedImage();
  typename MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  // Scale estimator is not used in current implementation yet
  typename ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();

  ImageMetricType * theMetric = dynamic_cast< ImageMetricType * >( this->m_theItkFilter->GetModifiableMetric() );

  auto optimizer = dynamic_cast< itk::GradientDescentOptimizerv4 * >( this->m_theItkFilter->GetModifiableOptimizer() );
  //auto optimizer = dynamic_cast<itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > *>(this->m_theItkFilter->GetModifiableOptimizer());

  typedef itk::Vector< TransformInternalComputationValueType, Dimensionality > VectorType;
  VectorType zeroVector( 0.0 );

  typedef itk::Image< VectorType, Dimensionality > DisplacementFieldType;
  typename DisplacementFieldType::Pointer displacementField = DisplacementFieldType::New();
  displacementField->CopyInformation( fixedImage );
  displacementField->SetRegions( fixedImage->GetBufferedRegion() );
  displacementField->Allocate();
  displacementField->FillBuffer( zeroVector );

  typename DisplacementFieldType::Pointer inverseDisplacementField = DisplacementFieldType::New();
  inverseDisplacementField->CopyInformation( fixedImage );
  inverseDisplacementField->SetRegions( fixedImage->GetBufferedRegion() );
  inverseDisplacementField->Allocate();
  inverseDisplacementField->FillBuffer( zeroVector );

  typedef typename TheItkFilterType::OutputTransformType OutputTransformType;
  typename OutputTransformType::Pointer outputTransform = OutputTransformType::New();
  outputTransform->SetDisplacementField( displacementField );
  outputTransform->SetInverseDisplacementField( inverseDisplacementField );

  this->m_theItkFilter->SetInitialTransform( outputTransform );
  this->m_theItkFilter->InPlaceOn();

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

  //this->m_theItkFilter->SetOptimizer(optimizer);

  // Below some hard coded options. Eventually, these should be part of new components.
  this->m_theItkFilter->SetNumberOfLevels( 3 );

  // Shrink the virtual domain by specified factors for each level.  See documentation
  // for the itkShrinkImageFilter for more detailed behavior.
  typename TheItkFilterType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize( 3 );
  shrinkFactorsPerLevel[ 0 ] = 4;
  shrinkFactorsPerLevel[ 1 ] = 2;
  shrinkFactorsPerLevel[ 2 ] = 1;
  this->m_theItkFilter->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );

  // Smooth by specified gaussian sigmas for each level.  These values are specified in
  // physical units.
  typename TheItkFilterType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize( 3 );
  smoothingSigmasPerLevel[ 0 ] = 4;
  smoothingSigmasPerLevel[ 1 ] = 2;
  smoothingSigmasPerLevel[ 2 ] = 1;
  this->m_theItkFilter->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );

  // TODO for now we hard code the TransformAdaptors for DisplacementFieldTransform.

  typedef itk::DisplacementFieldTransformParametersAdaptor< OutputTransformType > DisplacementFieldTransformAdaptorType;

  typename TheItkFilterType::TransformParametersAdaptorsContainerType adaptors;

  for( unsigned int level = 0; level < shrinkFactorsPerLevel.Size(); level++ )
  {
    // We use the shrink image filter to calculate the fixed parameters of the virtual
    // domain at each level.  To speed up calculation and avoid unnecessary memory
    // usage, we could calculate these fixed parameters directly.

    typedef itk::ShrinkImageFilter< FixedImageType, FixedImageType > ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
    shrinkFilter->SetShrinkFactors( shrinkFactorsPerLevel[ level ] );
    shrinkFilter->SetInput( fixedImage );
    shrinkFilter->Update();

    typename DisplacementFieldTransformAdaptorType::Pointer fieldTransformAdaptor = DisplacementFieldTransformAdaptorType::New();
    fieldTransformAdaptor->SetRequiredSpacing( shrinkFilter->GetOutput()->GetSpacing() );
    fieldTransformAdaptor->SetRequiredSize( shrinkFilter->GetOutput()->GetBufferedRegion().GetSize() );
    fieldTransformAdaptor->SetRequiredDirection( shrinkFilter->GetOutput()->GetDirection() );
    fieldTransformAdaptor->SetRequiredOrigin( shrinkFilter->GetOutput()->GetOrigin() );

    adaptors.push_back( fieldTransformAdaptor.GetPointer() );
  }

  /*
  typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
  fieldTransformAdaptor->SetRequiredSpacing(fixedImage->GetSpacing());
  fieldTransformAdaptor->SetRequiredSize(fixedImage->GetBufferedRegion().GetSize());
  fieldTransformAdaptor->SetRequiredDirection(fixedImage->GetDirection());
  fieldTransformAdaptor->SetRequiredOrigin(fixedImage->GetOrigin());

  adaptors.push_back(fieldTransformAdaptor.GetPointer());
  */
  this->m_theItkFilter->SetTransformParametersAdaptorsPerLevel( adaptors );

  typedef CommandIterationUpdate< TheItkFilterType > RegistrationCommandType;
  typename RegistrationCommandType::Pointer registrationObserver = RegistrationCommandType::New();
  this->m_theItkFilter->AddObserver( itk::IterationEvent(), registrationObserver );

  // perform the actual registration
  this->m_theItkFilter->Update();
}


template< int Dimensionality, class TPixel >
typename ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >::TransformPointer
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >
::GetItkTransform()
{
  return this->m_theItkFilter->GetModifiableTransform();
}


template< int Dimensionality, class TPixel >
bool
ItkSyNImageRegistrationMethodComponent< Dimensionality, TPixel >
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
  return meetsCriteria;
}
} //end namespace selx
