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

#include "selxNiftyregf3dComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
Niftyregf3dComponent< TPixel >::Niftyregf3dComponent(const std::string & name) : SuperElastixComponent(name)
{
  m_reg_f3d = new reg_f3d<TPixel>(1,1);
}


template< class TPixel >
Niftyregf3dComponent< TPixel >::~Niftyregf3dComponent()
{
    delete m_reg_f3d;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Set( typename NiftyregReferenceImageInterface< TPixel >::Pointer component )
{
  auto referenceimage = component->GetReferenceNiftiImage();
  // connect the itk pipeline
  this->m_reg_f3d->SetReferenceImage( referenceimage );
  return 0;
}

template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Set( typename NiftyregFloatingImageInterface< TPixel >::Pointer component)
{
  auto floatingimage = component->GetFloatingNiftiImage();
  // connect the itk pipeline
  this->m_reg_f3d->SetFloatingImage( floatingimage );
  return 0;
}



template< class TPixel >
void
Niftyregf3dComponent<  TPixel >
::RunRegistration()
{
  this->m_reg_f3d->UseSSD(0, true);
  this->m_reg_f3d->UseCubicSplineInterpolation();
  this->m_reg_f3d->Run();
  
}
  
template< class TPixel >
bool
Niftyregf3dComponent<  TPixel >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(false);
  auto status = CheckTemplateProperties(this->TemplateProperties(), criterion);
  if (status == CriterionStatus::Satisfied)
  {
    return true;
  }
  else if (status == CriterionStatus::Failed)
  {
    return false;
  } // else: CriterionStatus::Unknown

  return meetsCriteria;
}
} //end namespace selx
