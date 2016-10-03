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

#ifndef InterfaceTraits_h
#define InterfaceTraits_h

//TODO: note, maybe this functionality shouldn't be called a Trait, since we use a method ::Get(). Is Policy a better name?

#include "selxInterfaces.h"
#include "selxStaticErrorMessageRevealT.h"
#include "selxPodString.h"
#include "selxKeys.h"
//#include <type_traits>

namespace selx
{
// Traits to get printable interface name

// In our toolbox for each InterfaceType it is required to implement InterfaceName<InterfaceType>::Get()
// If one omits this, the implementation falls back to the default implementation that provides a compiler error message including the InterfaceType name for which the specialization is missing.
template< typename T >
struct Properties
{
  static_assert( StaticErrorMessageRevealT< T >::False, "Please Implement Properties<InterfaceType> for this InterfaceType" );
};

// Properties<T>::Get() should return the same name no matter whether T is an acceptor or provider interface.
template< typename InterfaceType >
struct Properties< InterfaceAcceptor< InterfaceType >>
{
  static const std::map< std::string, std::string > Get()
  {
    return Properties< InterfaceType >::Get();
  }
};

// The specializations for each type of Interface supported by the toolbox

template< >
struct Properties< MetricValueInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "MetricValueInterface" } };
  }
};
template< >
struct Properties< MetricDerivativeInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "MetricDerivativeInterface" } };
  }
};
template< >
struct Properties< OptimizerUpdateInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "OptimizerUpdateInterface" } };
  }
};

template< >
struct Properties< ConflictinUpdateInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "ConflictinUpdateInterface" } };
  }
};

template< >
struct Properties< TransformedImageInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "TransformedImageInterface" } };
  }
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

template< >
struct Properties< SourceInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "SourceInterface" } };
  }
};

template< >
struct Properties< SinkInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "SinkInterface" } };
  }
};

template< int D, class TPixel, class InternalComputationValueType >
struct Properties< itkMetricv4Interface< D, TPixel, InternalComputationValueType >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkMetricv4Interface" }, { keys::Dimensionality, std::to_string( D ) }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() } };
  }
};

template< class InternalComputationValueType >
struct Properties< itkOptimizerv4Interface< InternalComputationValueType >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkOptimizerv4Interface" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() } };
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

template< class F, class M >
struct Properties< elastixTransformParameterObjectInterface< F, M >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "elastixTransformParameterObjectInterface" } }; // TODO map F and M to strings?
  }
};

template< >
struct Properties< RegistrationControllerStartInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "RegistrationControllerStartInterface" } };
  }
};

template< >
struct Properties< RunRegistrationInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "RunRegistrationInterface" } };
  }
};

template< >
struct Properties< AfterRegistrationInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "AfterRegistrationInterface" } };
  }
};

template< >
struct Properties< ReconnectTransformInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "ReconnectTransformInterface" } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkTransformParametersAdaptorsContainerInterface" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( D ) } };
  }
};

template< class InternalComputationValueType, int D >
struct Properties< itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( D ) } };
  }
};
} // end namespace selx
#endif // #define InterfaceTraits_h
