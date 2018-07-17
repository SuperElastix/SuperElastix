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

#ifndef selxRegistrationMethodv4Interfaces_h
#define selxRegistrationMethodv4Interfaces_h

#include <string>

#include "itkProcessObject.h"
#include "itkImageToImageFilter.h"
#include "itkImageToImageMetricv4.h"
#include "itkObjectToObjectOptimizerBase.h"
#include "itkTransformParametersAdaptorBase.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"

#include "itkCompositeTransform.h"

#include "itkImage.h"
#include "itkMesh.h"

namespace selx
{
// Define the providing interfaces abstractly

template< int Dimensionality, class TPixel, class InternalComputationValueType >
class itkMetricv4Interface
{
public:

  using Type             = itkMetricv4Interface< Dimensionality, TPixel, InternalComputationValueType >;
  using Pointer          = std::shared_ptr< Type >;
  using FixedImageType   = itk::Image< TPixel, Dimensionality >;
  using MovingImageType  = FixedImageType;
  using VirtualImageType = FixedImageType;
  typedef typename itk::ImageToImageMetricv4< FixedImageType, MovingImageType, VirtualImageType,
    InternalComputationValueType > ImageToImageMetricv4Type;

  virtual typename ImageToImageMetricv4Type::Pointer GetItkMetricv4() = 0;
};

template< class TInternalComputationValueType >
class itkOptimizerv4Interface
{
public:

  using Type    = itkOptimizerv4Interface< TInternalComputationValueType >;
  using Pointer = std::shared_ptr< Type >;

  /**  Type of the optimizer. */
  typedef TInternalComputationValueType                                            InternalComputationValueType;
  typedef itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > OptimizerType;
  typedef typename OptimizerType::Pointer                                          Optimizerv4Pointer;
  virtual Optimizerv4Pointer GetItkOptimizerv4() = 0;
};

template< class TInternalComputationValueType, int Dimensionality >
class itkTransformInterface
{
public:

  using Type    = itkTransformInterface< TInternalComputationValueType, Dimensionality >;
  using Pointer = std::shared_ptr< Type >;
  typedef TInternalComputationValueType                                                            InternalComputationValueType;
  typedef typename itk::Transform< TInternalComputationValueType, Dimensionality, Dimensionality > TransformType;
  typedef typename TransformType::Pointer                                                          TransformPointer;

  virtual TransformPointer GetItkTransform() = 0;
};

template< class TransformInternalComputationValueType, int Dimensionality >
class itkTransformParametersAdaptorsContainerInterface
{
public:

  using Type    = itkTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType, Dimensionality >;
  using Pointer = std::shared_ptr< Type >;
  // TransformBaseType acts as a container of the types: TParametersValueType, NInputDimensions, NOutputDimensions
  using TransformBaseType                        = itk::Transform< TransformInternalComputationValueType, Dimensionality, Dimensionality >;
  using TransformParametersAdaptorBaseType       = itk::TransformParametersAdaptorBase< TransformBaseType >;
  using TransformParametersAdaptorsContainerType = std::vector< typename TransformParametersAdaptorBaseType::Pointer >;

  virtual TransformParametersAdaptorsContainerType GetItkTransformParametersAdaptorsContainer() = 0;
};

template< class TransformInternalComputationValueType, int Dimensionality >
class itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface
{
public:

  using Type
      = itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType,
    Dimensionality >;
  using Pointer = std::shared_ptr< Type >;

  using TransformBaseType                  = itk::Transform< TransformInternalComputationValueType, Dimensionality, Dimensionality >;
  using TransformParametersAdaptorBaseType = itk::TransformParametersAdaptorBase< TransformBaseType >;

  using GaussianExponentialDiffeomorphicTransformType
      = itk::GaussianExponentialDiffeomorphicTransform< TransformInternalComputationValueType, Dimensionality >;

  using TransformParametersAdaptorType
      = itk::GaussianExponentialDiffeomorphicTransformParametersAdaptor<
    GaussianExponentialDiffeomorphicTransformType >;
  using TransformParametersAdaptorPointer = typename TransformParametersAdaptorType::Pointer;
  //using TransformParametersAdaptorsContainerType = std::vector<TransformParametersAdaptorPointer>;
  using TransformParametersAdaptorsContainerType = std::vector< typename TransformParametersAdaptorBaseType::Pointer >;

  //using TransformParametersAdaptorsContainerType = typename TransformParametersAdaptorType::TransformParametersAdaptorsContainerType;

  virtual TransformParametersAdaptorsContainerType GetItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainer() = 0;
};

template< class InternalComputationValueType, int Dimensionality >
class MultiStageTransformInterface
{
public:

  using Type    = MultiStageTransformInterface< InternalComputationValueType, Dimensionality >;
  using Pointer = std::shared_ptr< Type >;

  using TransformBaseType
      = itk::Transform< InternalComputationValueType, Dimensionality, Dimensionality >;
  using CompositeTransformType
      = itk::CompositeTransform< InternalComputationValueType,
    Dimensionality >;
  virtual void SetFixedInitialTransform( typename CompositeTransformType::Pointer ) = 0;

  // TODO put this in a separate UpdateInterface that needs to connect too.
  virtual void Update( void ) = 0;

  virtual void SetMovingInitialTransform( typename CompositeTransformType::Pointer ) = 0;

  virtual typename TransformBaseType::Pointer GetItkTransform() = 0;

  // This method is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

template< int D, class TPixel, class InternalComputationValueType >
struct Properties< itkMetricv4Interface< D, TPixel, InternalComputationValueType >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkMetricv4Interface" }, { keys::Dimensionality, std::to_string( D ) },
             { keys::PixelType, PodString< TPixel >::Get() },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() } };
  }
};

template< class InternalComputationValueType >
struct Properties< itkOptimizerv4Interface< InternalComputationValueType >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkOptimizerv4Interface" },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< itkTransformInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkTransformInterface" }, { keys::Dimensionality, std::to_string( D ) } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< MultiStageTransformInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "MultiStageTransformInterface" }, { keys::Dimensionality, std::to_string( D ) } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkTransformParametersAdaptorsContainerInterface" },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() },
             { keys::Dimensionality, std::to_string( D ) } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface" },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() },
             { keys::Dimensionality, std::to_string( D ) } };
  }
};
} // end namespace selx

#endif // #define selxRegistrationMethodv4Interfaces_h
