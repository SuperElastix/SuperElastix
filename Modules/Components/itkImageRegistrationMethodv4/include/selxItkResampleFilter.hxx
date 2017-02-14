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

#include "selxItkResampleFilter.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel, class TInternalComputationValue >
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::ItkResampleFilterComponent(const std::string & name) : SuperElastixComponent(name)
{
  m_ResampleFilter = ResampleFilterType::New();
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::~ItkResampleFilterComponent()
{
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
int
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Set(typename itkImageDomainFixedInterface< Dimensionality >::Pointer component)
{
  auto fixedImageDomain = component->GetItkImageDomainFixed();
  // connect the itk pipeline

  //this->m_ResampleFilter->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  this->m_ResampleFilter->SetSize( fixedImageDomain->GetLargestPossibleRegion().GetSize() );  //should be virtual image...
  this->m_ResampleFilter->SetOutputOrigin( fixedImageDomain->GetOrigin() );
  this->m_ResampleFilter->SetOutputSpacing( fixedImageDomain->GetSpacing() );
  this->m_ResampleFilter->SetOutputDirection( fixedImageDomain->GetDirection() );
  this->m_ResampleFilter->SetDefaultPixelValue( 0 );
  return 0;
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
int
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Set(typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component)
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_ResampleFilter->SetInput( movingImage );
  return 0;
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
int
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Set(typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer component)
{
  //Store interface for later use
  this->m_TransformComponent = component;

  auto transform = component->GetItkTransform();
  // connect the itk pipeline
  this->m_ResampleFilter->SetTransform( transform );

  return 0;
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
typename ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::ResultImageType::Pointer
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::GetItkImage()
{
  return this->m_ResampleFilter->GetOutput();
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
void
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::ReconnectTransform()
{
  auto transform = this->m_TransformComponent->GetItkTransform();
  // reconnect the tranform, since it does not comply with the itk pipeline
  this->m_ResampleFilter->SetTransform( transform );
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
bool
ItkResampleFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  } // else: CriterionStatus::Unknown
  return meetsCriteria;
}
} //end namespace selx
