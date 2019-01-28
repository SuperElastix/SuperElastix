/*=========================================================================
 *
 *  This PreliminaryAffine Strategy Module developed by Rupert Brooks in the context
 *  of the Continuous Registration Challenge.  June 2018
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

#include "selxPreliminaryAffineStrategyRegistrationComponent.h"
#include "selxCheckTemplateProperties.h"
#include <string>
#include "PreliminaryAffineStrategy.h"
namespace selx
{
template< int Dimensionality, class TPixel >
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::PreliminaryAffineStrategyRegistrationComponent( const std::string & name,
  LoggerImpl & logger ) : Superclass( name, logger ), m_userMode(-1)
{
	this->m_DisplacementField = DisplacementFieldType::New();
}


template< int Dimensionality, class TPixel >
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::~PreliminaryAffineStrategyRegistrationComponent()
{
}


template< int Dimensionality, class TPixel >
int
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
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
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_ImageMovingInterface = component;
  return 0;
}


template< int Dimensionality, class TPixel >
typename PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::DisplacementFieldType::Pointer
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::GetItkDisplacementField()
{

  return this->m_DisplacementField;
}

template< int Dimensionality, class TPixel >
void
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >::Update( void )
{
  auto fixed = this->m_ImageFixedInterface->GetItkImageFixed();
  auto moving = this->m_ImageMovingInterface->GetItkImageMoving();
  
  fixed->SetRequestedRegionToLargestPossibleRegion();
  fixed->Update();
  // Raw pixel data is accessible by GetBufferPointer()
  moving->SetRequestedRegionToLargestPossibleRegion();
  moving->Update();
  // Raw pixel data is accessible by GetBufferPointer()
  PreliminaryAffineStrategy<TPixel,Dimensionality> p;
  p.m_strategy=m_strategy;
  p.m_log=[this](const int lvl,const std::string & message)->void {this->m_Logger.Log(LogLevel::TRC,message);};
  p.ProcessImages(fixed,moving,this->m_DisplacementField);

  // Here you can implement your registration algorithm using the fixed and moving image.
  // For an Identity Transform it is just creating a vector field with zeros.
  this->m_DisplacementField->Allocate();
}

template< int Dimensionality, class TPixel >
bool
PreliminaryAffineStrategyRegistrationComponent< Dimensionality, TPixel >
::MeetsCriterion( const CriterionType & criterion )
{
  // Each criterion is a property key-value pair set at the component by the user and is handled here one at the time.
  // First, check if the key is a template parameter and if the value corresponds to this component's instantiation.
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true; // The key is recognized and the value is correct
  }
  else if( status == CriterionStatus::Failed )
  {
    return false; // The key is recognized, but the value is incorrect
  } // else: CriterionStatus::Unknown

  std::ostringstream oss;
  for (auto c : criterion.second) oss << " " << c;

  // this->m_Logger.Log(LogLevel::TRC, "Got criterion \"{0}\": \"{1}\" ", criterion.first, oss.str());
  if (criterion.first == "UserMode")
  {
     m_userMode=std::stoi(criterion.second[0]);
  } else if (criterion.first == "Strategy")
  {
    m_strategy=criterion.second[0];
  }
  
  return true;
}
} //end namespace selx
