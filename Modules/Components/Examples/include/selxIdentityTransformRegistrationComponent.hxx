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

#include "selxIdentityTransformRegistrationComponent.h"
#include "selxCheckTemplateProperties.h"
#include <string>

namespace selx
{
template< int Dimensionality, class TPixel >
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::IdentityTransformRegistrationComponent( const std::string & name,
  LoggerImpl & logger ) : Superclass( name, logger )
{
	this->m_DisplacementField = DisplacementFieldImageType::New();
}


template< int Dimensionality, class TPixel >
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::~IdentityTransformRegistrationComponent()
{
}


template< int Dimensionality, class TPixel >
int
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  // Since SuperElastix is an itk filter in a pipeline, we need to set the domain of the resulting deformation field before running the registration.
  // As done in pairwise registration, typically, we use the domain of the fixed image for that.
  auto fixed = component->GetItkImageFixed();
  fixed->UpdateOutputInformation();
  auto size = fixed->GetLargestPossibleRegion().GetSize();
  this->m_DisplacementField->SetRegions(size);

  auto spacing = fixed->GetSpacing();
  this->m_DisplacementField->SetSpacing(spacing);

  auto origin = fixed->GetOrigin();
  this->m_DisplacementField->SetOrigin(origin);

  auto direction = fixed->GetDirection();
  this->m_DisplacementField->SetDirection(direction);

  this->m_ImageFixedInterface = component;
  return 0;
}


template< int Dimensionality, class TPixel >
int
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_ImageMovingInterface = component;
  return 0;
}


template< int Dimensionality, class TPixel >
typename IdentityTransformRegistrationComponent< Dimensionality, TPixel >::DisplacementFieldImageType::Pointer
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::GetDisplacementFieldItkImage()
{

  return this->m_DisplacementField;
}

template< int Dimensionality, class TPixel >
void
IdentityTransformRegistrationComponent< Dimensionality, TPixel >::RunRegistration( void )
{
  auto fixed = this->m_ImageFixedInterface->GetItkImageFixed();
  auto moving = this->m_ImageMovingInterface->GetItkImageMoving();
  
  fixed->SetRequestedRegionToLargestPossibleRegion();
  fixed->Update();
  // Raw pixel data is accessible by GetBufferPointer()
  moving->SetRequestedRegionToLargestPossibleRegion();
  moving->Update();
  // Raw pixel data is accessible by GetBufferPointer()
  
  // Here you can implement your registration algorithm using the fixed and moving image.
  // For an Identity Transform it is just creating a vector field with zeros.
  this->m_DisplacementField->Allocate();
}


template< int Dimensionality, class TPixel >
bool
IdentityTransformRegistrationComponent< Dimensionality, TPixel >
::MeetsCriterion( const CriterionType & criterion )
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
