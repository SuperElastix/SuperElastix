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

#ifndef selxCropperComponent_h
#define selxCropperComponent_h

#include "itkConnectedComponentImageFilter.h"
#include "itkLabelGeometryImageFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"

#include "selxSuperElastixComponent.h"
#include "selxItkObjectInterfaces.h"

namespace selx {
template< int Dimensionality, class TPixel >
class CropperComponent :
  public SuperElastixComponent<
    Accepting<
      itkImageInterface< Dimensionality, TPixel >,
      itkImageMaskInterface< Dimensionality, unsigned char >,
      itkImageFixedMaskInterface< Dimensionality, unsigned char >,
      itkImageMovingMaskInterface< Dimensionality, unsigned char >
    >,
    Providing<
      itkImageInterface< Dimensionality, TPixel >,
      itkImageFixedInterface< Dimensionality, TPixel >,
      itkImageDomainFixedInterface< Dimensionality >,
      itkImageMovingInterface< Dimensionality, TPixel >,
      UpdateInterface
    >
  >
{
public:
  using Self = CropperComponent< Dimensionality, TPixel >;
  using Pointer = std::shared_ptr< Self >;
  using ConstPointer = std::shared_ptr< const Self >;

  using Superclass = SuperElastixComponent<
    Accepting<
      itkImageInterface< Dimensionality, TPixel >,
      itkImageMaskInterface< Dimensionality, unsigned char >,
      itkImageFixedMaskInterface< Dimensionality, unsigned char >,
      itkImageMovingMaskInterface< Dimensionality, unsigned char >
    >,
    Providing<
      itkImageInterface< Dimensionality, TPixel >,
      itkImageFixedInterface< Dimensionality, TPixel >,
      itkImageDomainFixedInterface< Dimensionality >,
      itkImageMovingInterface< Dimensionality, TPixel >,
      UpdateInterface
    >
  >;

  CropperComponent( const std::string & name, LoggerImpl & logger );

  using ItkImageType = itk::Image< TPixel, Dimensionality >;
  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;
  using ItkImageDomainPointer = typename ItkImageDomainType::Pointer;
  using ItkImagePointer = typename ItkImageType::Pointer;
  using ItkImageMaskType = itk::Image< unsigned char, Dimensionality >;
  using ItkImageMaskPointer = typename ItkImageMaskType::Pointer;
  using LabelGeometryImageFilterType = itk::LabelGeometryImageFilter< ItkImageMaskType >;
  using LabelGeometryImageFilterPointer = typename LabelGeometryImageFilterType::Pointer;
  using RegionOfInterestImageFilterType = itk::RegionOfInterestImageFilter< ItkImageType, ItkImageType >;
  using RegionOfInterestImageFilterPointer = typename RegionOfInterestImageFilterType::Pointer;

  using CriterionType = ComponentBase::CriterionType;

  // Accepting
  int Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer ) override;
  int Accept( typename itkImageMaskInterface< Dimensionality, unsigned char >::Pointer ) override;
  int Accept( typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::Pointer ) override;
  int Accept( typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::Pointer ) override;

  // Providing
  ItkImagePointer GetItkImage() override;
  ItkImagePointer GetItkImageFixed() override;
  ItkImagePointer GetItkImageMoving() override;
  ItkImageDomainPointer GetItkImageDomainFixed() override;

  // Base methods
  void BeforeUpdate() override;
  void Update() override;
  bool MeetsCriterion( const CriterionType & criterion ) override;
  bool ConnectionsSatisfied() override;
  static const char* GetDescription() { return "ItkCropper Component"; }

private:

  LabelGeometryImageFilterPointer m_LabelGeometryImageFilter;
  RegionOfInterestImageFilterPointer m_RegionOfInterestImageFilter;

  int m_Pad;
  ItkImagePointer m_Image;
  ItkImageMaskPointer m_Mask;

protected:

  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkCropperComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }

};


} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxCropperComponent.hxx"
#endif

#endif // #define selxCropperComponent_h