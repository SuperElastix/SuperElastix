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
#ifndef selxNiftyregInterfaces_h
#define selxNiftyregInterfaces_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include <string.h>
//#include "_reg_f3d.h"
#include "_reg_ReadWriteImage.h"
#include "_reg_ReadWriteMatrix.h"

#include "selxInterfaceTraits.h"

namespace selx
{
template< class TPixel >
class NiftyregReferenceImageInterface
{
public:

  using Type    = NiftyregReferenceImageInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetReferenceNiftiImage() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregFloatingImageInterface
{
public:

  using Type    = NiftyregFloatingImageInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetFloatingNiftiImage() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregInputMaskInterface
{
public:

  using Type    = NiftyregInputMaskInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetInputMask() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregInputFloatingMaskInterface
{
public:

  using Type    = NiftyregInputFloatingMaskInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetInputFloatingMask() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregWarpedImageInterface
{
public:

  using Type    = NiftyregWarpedImageInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetWarpedNiftiImage() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregControlPointPositionImageInterface
{
public:

  using Type = NiftyregControlPointPositionImageInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetControlPointPositionImage() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
class NiftyregDisplacementFieldImageInterface
{
public:

  using Type = NiftyregDisplacementFieldImageInterface< TPixel >;
  using Pointer = std::shared_ptr< Type >;
  virtual std::shared_ptr< nifti_image > GetDisplacementFieldNiftiImage() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

class NiftyregAffineMatrixInterface
{
public:

  using Type = NiftyregAffineMatrixInterface;
  using Pointer = std::shared_ptr< Type >;
  virtual mat44 * GetAffineNiftiMatrix() = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< class TPixel >
struct Properties< NiftyregReferenceImageInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregReferenceImageInterface" }, { keys::PixelType, PodString< TPixel >::Get() }  };
  }
};

template< class TPixel >
struct Properties< NiftyregFloatingImageInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregFloatingImageInterface" }, { keys::PixelType, PodString< TPixel >::Get() }  };
  }
};

template< class TPixel >
struct Properties< NiftyregInputMaskInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregInputMaskInterface" }, { keys::PixelType, PodString< TPixel >::Get() }  };
  }
};

template< class TPixel >
struct Properties< NiftyregInputFloatingMaskInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregInputFloatingMaskInterface" }, { keys::PixelType, PodString< TPixel >::Get() }  };
  }
};

template< class TPixel >
struct Properties< NiftyregWarpedImageInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregWarpedImageInterface" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< class TPixel >
struct Properties< NiftyregControlPointPositionImageInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return{ { keys::NameOfInterface, "NiftyregControlPointPositionImageInterface" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template< class TPixel >
struct Properties< NiftyregDisplacementFieldImageInterface< TPixel >>
{
  static const std::map< std::string, std::string > Get()
  {
    return{ { keys::NameOfInterface, "NiftyregDisplacementFieldImageInterface" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};

template<>
struct Properties< NiftyregAffineMatrixInterface>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "NiftyregAffineMatrixInterface" } };
  }
};

}

#endif // #define selxNiftyregInterfaces_h
