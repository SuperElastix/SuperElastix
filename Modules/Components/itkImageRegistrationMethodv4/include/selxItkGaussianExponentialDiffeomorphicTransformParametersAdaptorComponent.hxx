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

#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent.h"

namespace selx
{

template< int Dimensionality, class TransformInternalComputationValueType >
ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >::ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent()
{
  m_theItkFilter = TheItkFilterType::New();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TransformInternalComputationValueType >
ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >::~ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent()
{
}


template< int Dimensionality, class TransformInternalComputationValueType >
int
ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >
::Set( itkImageDomainFixedInterface< Dimensionality, TransformInternalComputationValueType > * component )
{
  auto fixedImageDomain = component->GetItkImageDomainFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage(fixedImageDomain);

  return 0;
}



  // TODO for now we hard code the TransformAdaptors for stationary velocity fields.
  typedef double                                                                                                RealType;
  typedef itk::GaussianExponentialDiffeomorphicTransform< RealType, Dimensionality >                            ConstantVelocityFieldTransformType;
  typedef typename ConstantVelocityFieldTransformType::ConstantVelocityFieldType                                ConstantVelocityFieldType;
  typedef itk::GaussianExponentialDiffeomorphicTransformParametersAdaptor< ConstantVelocityFieldTransformType > VelocityFieldTransformAdaptorType;

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

    typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
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


template< int Dimensionality, class TransformInternalComputationValueType >
typename ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >::TransformPointer
ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >
::GetItkTransform()
{
  return this->m_theItkFilter->GetModifiableTransform();
}


template< int Dimensionality, class TransformInternalComputationValueType >
bool
ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorComponent< Dimensionality, TransformInternalComputationValueType >
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
