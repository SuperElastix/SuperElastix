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

#ifndef selxItkObjectInterfaces_h
#define selxItkObjectInterfaces_h

#include "itkImage.h"
#include "itkMesh.h"


namespace selx
{
// Define the providing interfaces abstractly

  template< int Dimensionality, class TPixel >
  class itkImageInterface
  {
    // An interface that provides the pointer of an output image

  public:
    using Type = itkImageInterface< Dimensionality, TPixel >;
    using Pointer = std::shared_ptr<Type>;
    typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
    virtual typename ItkImageType::Pointer GetItkImage() = 0;
  };

  template< int Dimensionality, class TPixel >
  class itkImageFixedInterface
  {
    // An interface that provides the smart pointer to an itk image

  public:
    using Type = itkImageFixedInterface< Dimensionality, TPixel >;
    using Pointer = std::shared_ptr<Type>;
    typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
    virtual typename ItkImageType::Pointer GetItkImageFixed() = 0;
  };

  template< int Dimensionality >
  class itkImageDomainFixedInterface
  {
    // An interface that provides the smart pointer to the base class of an itk image
    // that holds the origin/spacing/ etc domain information.

  public:
    using Type = itkImageDomainFixedInterface< Dimensionality > ;
    using Pointer = std::shared_ptr<Type>;
    typedef typename itk::ImageBase< Dimensionality > ItkImageDomainType;
    virtual typename ItkImageDomainType::Pointer GetItkImageDomainFixed() = 0;
  };

  template< int Dimensionality, class TPixel >
  class itkImageMovingInterface
  {
    // An interface that provides the smart pointer to an itk image

  public:
    using Type = itkImageMovingInterface< Dimensionality, TPixel > ;
    using Pointer = std::shared_ptr<Type>;
    typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
    virtual typename ItkImageType::Pointer GetItkImageMoving() = 0;
  };

  template< int Dimensionality, class TPixel >
  class DisplacementFieldItkImageSourceInterface
  {
    // An interface that passes the pointer of an output image

  public:
    using Type = DisplacementFieldItkImageSourceInterface< Dimensionality, TPixel > ;
    using Pointer = std::shared_ptr<Type>;
    typedef typename itk::Image< itk::Vector< TPixel, Dimensionality >, Dimensionality > ItkImageType;
    virtual typename ItkImageType::Pointer GetDisplacementFieldItkImage() = 0;
  };

  template< int Dimensionality, class TPixel >
  class itkMeshInterface
  {
    // An interface that passes the pointer of an output mesh

  public:
    using Type = itkMeshInterface< Dimensionality, TPixel > ;
    using Pointer = std::shared_ptr<Type>;
    virtual typename itk::Mesh< TPixel, Dimensionality >::Pointer GetItkMesh() = 0;
  };

  

// InterfaceName<T>::Get() should return "itkImageSourceInterface" no matter over which arguments itkImageSourceInterface is templated
template< int D, class TPixel >
struct Properties< itkImageInterface< D, TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkImageInterface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< int D, class TPixel >
struct Properties< itkImageFixedInterface< D, TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkImageFixedInterface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< int D >
struct Properties< itkImageDomainFixedInterface< D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkImageDomainFixedInterface" }, { keys::Dimensionality, std::to_string( D ) } };
  }
};

template< int D, class TPixel >
struct Properties< itkImageMovingInterface< D, TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkImageMovingInterface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< int D, class TPixel >
struct Properties< DisplacementFieldItkImageSourceInterface< D, TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "DisplacementFieldItkImageSourceInterface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< int D, class TPixel >
struct Properties< itkMeshInterface< D, TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkMeshInterface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

} // end namespace selx

#endif // #define selxItkObjectInterfaces_h
