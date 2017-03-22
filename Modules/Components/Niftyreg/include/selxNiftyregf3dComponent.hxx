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
Niftyregf3dComponent< TPixel >::Niftyregf3dComponent( const std::string & name ) : Superclass( name )
{
  m_reg_f3d = new reg_f3d< TPixel >( 1, 1 );
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
  this->m_reg_f3d->SetReferenceImage( referenceimage.get() );
  return 0;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Set( typename NiftyregFloatingImageInterface< TPixel >::Pointer component )
{
  auto floatingimage = component->GetFloatingNiftiImage();
  // connect the itk pipeline
  this->m_reg_f3d->SetFloatingImage( floatingimage.get() );
  return 0;
}

template< class TPixel >
std::shared_ptr<nifti_image>
Niftyregf3dComponent< TPixel >
::GetWarpedNiftiImage()
{
  return (*(this->m_warped_images.get()))[0];
}


template< class TPixel >
void
Niftyregf3dComponent<  TPixel >
::RunRegistration()
{
  this->m_reg_f3d->UseSSD( 0, true );
  this->m_reg_f3d->UseCubicSplineInterpolation();

  this->m_reg_f3d->Run();
  nifti_image** outputWarpedImage = m_reg_f3d->GetWarpedImage();
  memset(outputWarpedImage[0]->descrip, 0, 80);
  strcpy(outputWarpedImage[0]->descrip, "Warped image using NiftyReg (reg_f3d)");

  //encapsulate malloc-ed pointer in a smartpointer for proper memory ownership
  this->m_warped_images = std::unique_ptr< std::array<std::shared_ptr<nifti_image>, 2>>(new std::array<std::shared_ptr<nifti_image>, 2>);
  (*(this->m_warped_images.get()))[0] = std::shared_ptr<nifti_image>(outputWarpedImage[0], nifti_image_free);
  (*(this->m_warped_images.get()))[1] = std::shared_ptr<nifti_image>(outputWarpedImage[1], nifti_image_free);

  free(outputWarpedImage);
  outputWarpedImage = NULL;

}


template< class TPixel >
bool
Niftyregf3dComponent<  TPixel >
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
