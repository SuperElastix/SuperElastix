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

#include "selxNiftiToItkImageSinkComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::NiftiToItkImageSinkComponent( const std::string & name, const LoggerInterface & logger ) : Superclass( name,
  logger), m_NetworkBuilderOutputImage(nullptr), m_WarpedImageInterface(nullptr), m_ImageDomainInterface(nullptr)
{
  m_MiniPipelineOutputImage = ItkImageType::New(); // this is just an empty image for we have a SmartPointer we can pass around downstream. The actual image data will be grafted into this image.
  m_ImportFilter = ImportFilterType::New();
}


template< int Dimensionality, class TPixel >
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::~NiftiToItkImageSinkComponent()
{
}


template< int Dimensionality, class TPixel >
int
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::Set(typename WarpedImageInterfaceType::Pointer other)
{
 
  // Store pointer to the m_WarpedImageInterface for getting the result image after in has been generated (registration).
  // TODO: sanity check that m_WarpedImageInterface was Null to detect if Set was called more than once erroneously.
  this->m_WarpedImageInterface = other;
  return 0;
}

template< int Dimensionality, class TPixel >
int
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::Set(typename ImageDomainInterfaceType::Pointer other)
{

  // Store pointer to the m_ImageDomainInterface for getting the result image after in has been generated (registration).
  // TODO: sanity check that m_ImageDomainInterface was Null to detect if Set was called more than once erroneously.
  m_MiniPipelineOutputImage->SetRegions(other->GetItkImageDomainFixed()->GetLargestPossibleRegion());
  //this->m_ImageDomainInterface = other;
  return 0;
}

template< int Dimensionality, class TPixel >
void
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::SetMiniPipelineOutput(itk::DataObject::Pointer NetworkBuilderOutput)
{
  /** Tries to cast the NetworkBuilderOutput to an image (data object) and stores the result.
  *  The resulting output image will be grafted into when the sink component is connected to an other component.
  * */
  //
  /*
  this->m_NetworkBuilderOutputImage = dynamic_cast< ItkImageType * >(NetworkBuilderOutput.GetPointer());
  if (this->m_NetworkBuilderOutputImage == nullptr)
  {
    throw std::runtime_error("NiftiToItkImageSinkComponent cannot cast the NetworkBuilder's Output to the required type");
  }
  this->m_MiniPipelineOutputImage = this->m_NetworkBuilderOutputImage;
  */
}


template< int Dimensionality, class TPixel >
typename itk::DataObject::Pointer
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::GetMiniPipelineOutput()
{
  return this->m_MiniPipelineOutputImage.GetPointer();
}

template< int Dimensionality, class TPixel >
typename AnyFileWriter::Pointer
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::GetOutputFileWriter()
{
  // Instanstiate an image file writer, decorated such that it can be implicitly cast to an AnyFileWriterType
  return DecoratedWriterType::New().GetPointer();
}


template< int Dimensionality, class TPixel >
typename itk::DataObject::Pointer
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::GetInitializedOutput()
{
  return ItkImageType::New().GetPointer();
}

template< int Dimensionality, class TPixel >
void
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::AfterRegistration()
{
  
  auto warpedNiftiImage = this->m_WarpedImageInterface->GetWarpedNiftiImage();
  auto warpedItkImage = NiftiToItkImage<ItkImageType, TPixel>::Convert(warpedNiftiImage);
  //TPixel *  	ptr;
  //SizeValueType  num;
  //std:tie(ptr,num) = NiftiToItkImage<ItkImageType, TPixel>::Convert(warpedNiftiImage);

  //this->m_ImportFilter->SetImportPointer(ptr, num, true);
  this->m_MiniPipelineOutputImage->Graft(warpedItkImage);
}

template< int Dimensionality, class TPixel >
bool
NiftiToItkImageSinkComponent< Dimensionality, TPixel >::MeetsCriterion(const ComponentBase::CriterionType & criterion)
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
