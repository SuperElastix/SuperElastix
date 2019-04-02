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

#include "selxItkTransformDisplacementFilterComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel, class TInternalComputationValue >
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::ItkTransformDisplacementFilterComponent(
  const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  m_DisplacementFieldFilter = DisplacementFieldFilterType::New();
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::~ItkTransformDisplacementFilterComponent()
{
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
int
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer component )
{
  this->m_ImageDomainFixed = component->GetItkImageDomainFixed();
  return 0;
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
int
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Accept( typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer component )
{
  this->m_TransformComponent = component;
  return 0;
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
typename ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >::DisplacementFieldType::Pointer
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::GetItkDisplacementField()
{
  return this->m_DisplacementFieldFilter->GetOutput();
}


template< int Dimensionality, class TPixel, class TInternalComputationValue >
void
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::BeforeUpdate()
{
  // Required to satisfy input requirements when UpdateOutputInformation is run
  this->m_DisplacementFieldFilter->SetTransform( this->m_TransformComponent->GetItkTransform() );

  this->m_DisplacementFieldFilter->SetSize( this->m_ImageDomainFixed->GetLargestPossibleRegion().GetSize() );
  this->m_DisplacementFieldFilter->SetOutputOrigin( this->m_ImageDomainFixed->GetOrigin() );
  this->m_DisplacementFieldFilter->SetOutputSpacing( this->m_ImageDomainFixed->GetSpacing() );
  this->m_DisplacementFieldFilter->SetOutputDirection( this->m_ImageDomainFixed->GetDirection() );
}

template< int Dimensionality, class TPixel, class TInternalComputationValue >
void
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
::Update() {
  // Required to retrieve final transform
  this->m_DisplacementFieldFilter->SetTransform( this->m_TransformComponent->GetItkTransform() );
};


template< int Dimensionality, class TPixel, class TInternalComputationValue >
bool
ItkTransformDisplacementFilterComponent< Dimensionality, TPixel, TInternalComputationValue >
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
