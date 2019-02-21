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

#include "selxItkToNiftiImageHybridSourceComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >
::ItkToNiftiImageHybridSourceComponent(const std::string & name, LoggerImpl & logger) : Superclass( name, logger ), m_Image( nullptr )
{
}


template< int Dimensionality, class TPixel >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::~ItkToNiftiImageHybridSourceComponent()
{
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetReferenceNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }

  this->m_Image->GetSource()->UpdateLargestPossibleRegion();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetFloatingNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }

  this->m_Image->GetSource()->UpdateLargestPossibleRegion();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetWarpedNiftiImage()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }

  this->m_Image->GetSource()->UpdateLargestPossibleRegion();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, TPixel >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetInputMask()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }

  this->m_Image->GetSource()->UpdateLargestPossibleRegion();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, unsigned char >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
std::shared_ptr< nifti_image >
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetInputFloatingMask()
{
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }

  this->m_Image->GetSource()->UpdateLargestPossibleRegion();

  // TODO memory management issue: the Convert function passes the ownership
  // of the data buffer from the itk image to the nifti image. This means that
  // as soon as the shared_ptr<nifti_image> goes out of scope the buffer is freed
  // and the itk image is invalidated. However, subsequently destructing the itk
  // image should be without memory leaks.

  return ItkToNiftiImage< ItkImageType, unsigned char >::Convert( this->m_Image );
}


template< int Dimensionality, class TPixel >
typename ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::ItkImageType::Pointer
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >
        ::GetItkImage()
{
  if (this->m_Image == nullptr)
  {
    throw std::runtime_error("SourceComponent needs to be initialized by SetMiniPipelineInput()");
  }
  return this->m_Image;
}

template< int Dimensionality, class TPixel >
typename ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::ItkImageDomainType::Pointer
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >
::GetItkImageDomainFixed()
{
  if (this->m_Image == nullptr)
  {
    throw std::runtime_error("SourceComponent needs to be initialized by SetMiniPipelineInput()");
  }
  return this->m_Image.GetPointer();
}

template< int Dimensionality, class TPixel >
void
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >
::SetMiniPipelineInput( itk::DataObject::Pointer object )
{
  this->m_Image = dynamic_cast< ItkImageType * >( object.GetPointer() );
  if( this->m_Image == nullptr )
  {
    throw std::runtime_error( "DataObject passed by the NetworkBuilder is not of the right ImageType or not at all an ImageType" );
  }
  return;
}


template< int Dimensionality, class TPixel >
typename AnyFileReader::Pointer
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::GetInputFileReader()
{
  // Instanstiate an image file reader, decorated such that it can be implicitly cast to an AnyFileReaderType
  return DecoratedReaderType::New().GetPointer();
}

template< int Dimensionality, class TPixel >
bool
ItkToNiftiImageHybridSourceComponent< Dimensionality, TPixel >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
