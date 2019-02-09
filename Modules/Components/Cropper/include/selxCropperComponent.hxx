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

namespace selx
{

template< int Dimensionality, class TPixel >
CropperComponent< Dimensionality, TPixel >::CropperComponent( const std::string & name,
                                                              LoggerImpl & logger ) : Superclass( name, logger ) {

  this->m_ConnectedComponentImageFilter
    = ConnectedComponentImageFilterType::New();

  this->m_LabelGeometryImageFilter
    = LabelGeometryImageFilterType::New();
  this->m_LabelGeometryImageFilter->SetInput(this->m_ConnectedComponentImageFilter->GetOutput());
  this->m_LabelGeometryImageFilter->CalculateOrientedBoundingBoxOn();

  this->m_ExtractImageFilter
    = ExtractImageFilterType::New();
  this->m_ExtractImageFilter->SetDirectionCollapseToIdentity();

  this->m_Pad = 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer component )
{
  this->m_Image = component->GetItkImage();
  this->m_ExtractImageFilter->SetInput(this->m_Image);
  return 0;
}

template< int Dimensionality, class TPixel >
int
CropperComponent< Dimensionality, TPixel >::Accept( typename itkImageMaskInterface< Dimensionality, unsigned char >::Pointer component )
{
  // We find the size of the extracted region here so ITK's pipeline know the output size
  this->m_ConnectedComponentImageFilter->SetInput(component->GetItkImageMask());

  return 0;
}

template< int Dimensionality, class TPixel >
typename CropperComponent< Dimensionality, TPixel >::ItkImagePointer
CropperComponent< Dimensionality, TPixel >::GetItkImage()
{
  this->m_LabelGeometryImageFilter->Update();
  auto boundingBox = this->m_LabelGeometryImageFilter->GetBoundingBox(1);

  typename itk::Index< Dimensionality > start;
  start[0] = std::min(boundingBox.GetElement(0) - this->m_Pad, long(0));
  start[1] = std::min(boundingBox.GetElement(2) - this->m_Pad, long(0));
  if( Dimensionality > 2 ) start[2] = std::min(boundingBox.GetElement(4) - this->m_Pad, long(0));
  if( Dimensionality > 3 ) start[3] = std::min(boundingBox.GetElement(6) - this->m_Pad, long(0));

  typename itk::Size< Dimensionality > size;
  auto imageSize = this->m_Image->GetBufferedRegion().GetSize();
  size[0] = std::max(boundingBox.GetElement(1) - start[0] + 1 + this->m_Pad, long(this->m_Image->GetBufferedRegion().GetSize(0)));
  size[1] = std::max(boundingBox.GetElement(3) - start[1] + 1 + this->m_Pad, long(this->m_Image->GetBufferedRegion().GetSize(1)));
  if( Dimensionality > 2 ) size[2] = std::max(boundingBox.GetElement(5) - start[2] + 1 + this->m_Pad, long(this->m_Image->GetBufferedRegion().GetSize(2)));
  if( Dimensionality > 3 ) size[3] = std::max(boundingBox.GetElement(7) - start[3] + 1 + this->m_Pad, long(this->m_Image->GetBufferedRegion().GetSize(3)));

  typename ItkImageType::RegionType region(start, size);
  this->m_ExtractImageFilter->SetExtractionRegion(region);

  return this->m_ExtractImageFilter->GetOutput();
}

template< int Dimensionality, class TPixel >
void
CropperComponent< Dimensionality, TPixel >::Update()
{


  this->m_ExtractImageFilter->Update();
}

template< int Dimensionality, class TPixel >
bool
CropperComponent< Dimensionality, TPixel >
::MeetsCriterion( const CriterionType & criterion )
{
  bool meetsCriteria( false );
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
  }

  return meetsCriteria;
};

}
