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

#include "selxItkToNiftiImageComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkToNiftiImageComponent< Dimensionality, TPixel >
::ItkToNiftiImageComponent(const std::string & name, LoggerImpl & logger) : Superclass( name, logger ), m_Image( nullptr )
{
}


template< int Dimensionality, class TPixel >
ItkToNiftiImageComponent< Dimensionality, TPixel >::~ItkToNiftiImageComponent()
{
}

template< int Dimensionality, class TPixel >
int
ItkToNiftiImageComponent< Dimensionality, TPixel >::Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer component)
{
  this->m_Image = component->GetItkImage();
  return 0;
}


template< int Dimensionality, class TPixel >
int
ItkToNiftiImageComponent< Dimensionality, TPixel >::Accept( typename itkImageMaskInterface< Dimensionality, TPixel >::Pointer component)
{
  this->m_Image = component->GetItkImageMask();
  return 0;
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageComponent< Dimensionality, TPixel >::GetReferenceNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "Image not set." );
  }

  this->m_Image->Update();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageComponent< Dimensionality, TPixel >::GetFloatingNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "Image not set." );
  }

  this->m_Image->Update();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageComponent< Dimensionality, TPixel >::GetWarpedNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "Image not set." );
  }

  this->m_Image->Update();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageComponent< Dimensionality, TPixel >::GetInputMask()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "Image not set." );
  }

  this->m_Image->Update();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}

template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageComponent< Dimensionality, TPixel >::GetInputFloatingMask()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "Image not set." );
  }

  this->m_Image->Update();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}



template< int Dimensionality, class TPixel >
bool
ItkToNiftiImageComponent< Dimensionality, TPixel >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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

template< int Dimensionality, class TPixel  >
bool
ItkToNiftiImageComponent< Dimensionality, TPixel >
::ConnectionsSatisfied() {
  if (this->InterfaceAcceptor<itkImageInterface<Dimensionality, TPixel >>::GetAccepted() ||
      this->InterfaceAcceptor<itkImageMaskInterface<Dimensionality, TPixel >>::GetAccepted()) {
    return true;
  }

  return false;
}

} //end namespace selx
