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

#include "selxCropperComponent.h"
#include "selxCheckTemplateProperties.h"

#include "itkImageFileWriter.h"

namespace selx
{

template< int Dimensionality, class TPixel >
CropperComponent< Dimensionality, TPixel >::CropperComponent( const std::string & name,
                                                              LoggerImpl & logger ) : Superclass( name, logger ) {
  this->m_LabelGeometryImageFilter
    = LabelGeometryImageFilterType::New();
  this->m_LabelGeometryImageFilter->CalculateOrientedBoundingBoxOn();

  this->m_RegionOfInterestImageFilter
    = RegionOfInterestImageFilterType::New();

  this->m_Pad = 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_Image = component->GetItkImage();
  this->m_RegionOfInterestImageFilter->SetInput(this->m_Image);
  return 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageMaskInterface< Dimensionality, unsigned char >::Pointer component )
{
  this->m_Mask = component->GetItkImageMask();
  return 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::Pointer component )
{
  this->m_Mask = component->GetItkImageFixedMask();
  return 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::Pointer component )
{
  this->m_Mask = component->GetItkImageMovingMask();
  return 0;
}

template< int Dimensionality, class TPixel >
typename CropperComponent< Dimensionality, TPixel >::ItkImagePointer
CropperComponent< Dimensionality, TPixel >::GetItkImage()
{
  return this->m_RegionOfInterestImageFilter->GetOutput();
}

template< int Dimensionality, class TPixel >
typename CropperComponent< Dimensionality, TPixel >::ItkImagePointer
CropperComponent< Dimensionality, TPixel >::GetItkImageFixed()
{
  return this->GetItkImage();
};

template< int Dimensionality, class TPixel >
typename CropperComponent< Dimensionality, TPixel >::ItkImagePointer
CropperComponent< Dimensionality, TPixel >::GetItkImageMoving()
{
  return this->GetItkImage();
};

template< int Dimensionality, class TPixel >
typename CropperComponent< Dimensionality, TPixel >::ItkImageDomainPointer
CropperComponent< Dimensionality, TPixel >
::GetItkImageDomainFixed()
{
  // Implicitly casted to domain (itk::ImageBase)
  return this->m_RegionOfInterestImageFilter->GetOutput();
}

template< int Dimensionality, class TPixel >
void
CropperComponent< Dimensionality, TPixel >::BeforeUpdate()
{

  this->m_Image->UpdateOutputInformation();
  this->m_Mask->UpdateOutputInformation();

  // Output information must be generated before downstream components are run
  this->m_LabelGeometryImageFilter->SetInput(this->m_Mask);
  this->m_LabelGeometryImageFilter->Update();
  const auto boundingBox = this->m_LabelGeometryImageFilter->GetBoundingBox(1);
  const auto& largestPossibleRegion = this->m_Image->GetLargestPossibleRegion();

  {
    std::stringstream ss;
    ss << largestPossibleRegion;
    this->m_Logger.Log(LogLevel::INF, "{0}: Got image with {1}", this->m_Name, ss.str());
  }

  itk::Index< Dimensionality > start;
  itk::Size< Dimensionality > size;

  for (unsigned i{ 0 }; i < Dimensionality; ++i)
  {
    start[i] = std::max<itk::IndexValueType>(
      boundingBox.GetElement(2 * i) - this->m_Pad, largestPossibleRegion.GetIndex(i));
    size[i] = std::min<itk::IndexValueType>(
      boundingBox.GetElement(2 * i + 1) - start[i] + 1 + this->m_Pad, largestPossibleRegion.GetSize(i) - start[i]);
  }

  const typename ItkImageType::RegionType croppedRegion(start, size);

  {
    std::stringstream ss;
    ss << croppedRegion;
    this->m_Logger.Log(LogLevel::INF, "{0}: Cropping image to {1}", this->m_Name, ss.str());
  }

  this->m_RegionOfInterestImageFilter->SetRegionOfInterest(croppedRegion);
}

template< int Dimensionality, class TPixel >
void
CropperComponent< Dimensionality, TPixel >::Update()
{

}

template< int Dimensionality, class TPixel >
bool
CropperComponent< Dimensionality, TPixel >
::MeetsCriterion( const CriterionType & criterion )
{
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  }

  if( criterion.first == "Pad" ) {
    this->m_Pad = std::stoi( criterion.second[0] );
    return true;
  }

  return false;
};

template< int Dimensionality, class TPixel  >
bool
CropperComponent< Dimensionality, TPixel >
::ConnectionsSatisfied() {
  if (!this->InterfaceAcceptor<itkImageInterface<Dimensionality, TPixel >>::GetAccepted()) {
    return false;
  }

  if (!this->InterfaceAcceptor<itkImageMaskInterface<Dimensionality, unsigned char >>::GetAccepted() &&
      !this->InterfaceAcceptor<itkImageFixedMaskInterface<Dimensionality, unsigned char >>::GetAccepted() &&
      !this->InterfaceAcceptor<itkImageMovingMaskInterface<Dimensionality, unsigned char >>::GetAccepted()) {
    return false;
  }

  return true;
}

}
