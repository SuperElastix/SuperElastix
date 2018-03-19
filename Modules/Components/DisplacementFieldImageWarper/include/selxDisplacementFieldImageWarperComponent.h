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

#ifndef selxDisplacementFieldImageWarperComponent_h
#define selxDisplacementFieldImageWarperComponent_h

#include "selxSuperElastixComponent.h"
#include "selxItkObjectInterfaces.h"

#include "itkDisplacementFieldTransform.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
class ItkDisplacementFieldImageWarperComponent : public
  SuperElastixComponent<
  Accepting< itkImageMovingInterface< Dimensionality, TPixel >, itkDisplacementFieldInterface< Dimensionality, CoordRepType > >,
  Providing< itkImageInterface< Dimensionality, TPixel > > >
{
public:
  typedef ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType > Self;
  typedef SuperElastixComponent<
    Accepting< itkImageMovingInterface< Dimensionality, TPixel >, itkDisplacementFieldInterface< Dimensionality, CoordRepType > >,
    Providing< itkImageInterface< Dimensionality, TPixel > > > Superclass;
  typedef std::shared_ptr< Self > Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldImageWarperComponent( const std::string & name, LoggerImpl & logger );

  using DisplacementFieldInterfaceType = typename itkDisplacementFieldInterface< Dimensionality, CoordRepType >::Type;
  using DisplacementFieldInterfacePointer = typename DisplacementFieldInterfaceType::Pointer;

  using DisplacementFieldType = typename DisplacementFieldInterfaceType::ItkDisplacementFieldType;
  using DisplacementFieldPointer = typename DisplacementFieldType::Pointer;

  using ImageInterfaceType = typename itkImageInterface< Dimensionality, TPixel >::Type;
  using ImageInterfacePointer = typename ImageInterfaceType::Pointer;

  using ResultImageType = typename ImageInterfaceType::ItkImageType;
  using ResultImagePointer = typename ResultImageType::Pointer;

  using MovingImageInterfaceType = typename itkImageMovingInterface< Dimensionality, TPixel >::Type;
  using MovingImageInterfacePointer = typename MovingImageInterfaceType::Pointer;

  using MovingImageType = typename MovingImageInterfaceType::ItkImageType;
  using MovingImagePointer = typename MovingImageType::Pointer;

  using DisplacementFieldTransformType = itk::DisplacementFieldTransform< CoordRepType, Dimensionality >;
  using DisplacementFieldTransformPointer = typename DisplacementFieldTransformType::Pointer;

  using ResampleImageFilterType = itk::ResampleImageFilter< MovingImageType, ResultImageType, CoordRepType >;
  using ResampleImageFilterPointer = typename ResampleImageFilterType::Pointer;

  using NearestNeighborInterpolatorType = itk::NearestNeighborInterpolateImageFunction<
          ResultImageType, typename ResultImageType::PixelType>;
  using NearestNeighborInterpolatorTypePointer = typename NearestNeighborInterpolatorType::Pointer;

  // Accept interfaces
  virtual int Accept( DisplacementFieldInterfacePointer ) override;
  virtual int Accept( MovingImageInterfacePointer ) override;

  // Provide interfaces
  virtual ResultImagePointer GetItkImage() override;

  // Base methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "Warp a point set based on a deformation field"; };

protected:

  // return the class name and the template arguments to identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return {
      { keys::NameOfClass, "ItkDisplacementFieldImageWarperComponent" },
      { keys::PixelType, PodString< TPixel >::Get() },
      { keys::CoordRepType, PodString< CoordRepType >::Get() },
      { keys::Dimensionality, std::to_string( Dimensionality ) }
    };
  }

private:

  DisplacementFieldTransformPointer m_DisplacementFieldTransform;
  ResampleImageFilterPointer m_ResampleImageFilter;

  bool m_UseNearestNeighborInterpolation;
};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldImageWarperComponent.hxx"
#endif

#endif // selxDisplacementFieldImageWarperComponent_h
