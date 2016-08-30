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
//#include <type_traits>

namespace selx
{
  // helper to display type name in static assert error message (required, at least for VC++ 2013)
  template <typename T>
  struct StaticErrorMessageRevealT { enum { False = false }; };
  

// Traits to get printable interface name

// In our toolbox for each InterfaceType it is required to implement InterfaceName<InterfaceType>::Get()
// If one omits this, the implementation falls back to the default implementation that provides a compiler error message including the InterfaceType name for which the specialization is missing.
template< typename T >
struct InterfaceName
{
  static_assert(StaticErrorMessageRevealT<T>::False, "Please Implement InterfaceName<InterfaceType> for this InterfaceType");
};

// The specializations for each type of Interface supported by the toolbox

template< >
struct InterfaceName< MetricValueInterface >
{
  static const char * Get()
  {
    return "MetricValueInterface";
  }
};
template< >
struct InterfaceName< MetricDerivativeInterface >
{
  static const char * Get()
  {
    return "MetricDerivativeInterface";
  }
};
template< >
struct InterfaceName< OptimizerUpdateInterface >
{
  static const char * Get()
  {
    return "OptimizerUpdateInterface";
  }
};

template< >
struct InterfaceName< ConflictinUpdateInterface >
{
  static const char * Get()
  {
    return "ConflictinUpdateInterface";
  }
};

template< >
struct InterfaceName< TransformedImageInterface >
{
  static const char * Get()
  {
    return "TransformedImageInterface";
  }
};

// InterfaceName<T>::Get() should return "itkImageSourceInterface" no matter over which arguments itkImageSourceInterface is templated
template< int D, class TPixel >
struct InterfaceName< itkImageInterface< D, TPixel >>
{
  static const char * Get()
  {
    return "itkImageInterface";
  }
};

template< int D, class TPixel >
struct InterfaceName< itkImageFixedInterface< D, TPixel >>
{
  static const char * Get()
  {
    return "itkImageFixedInterface";
  }
};

template< int D >
struct InterfaceName< itkImageDomainFixedInterface< D >>
{
  static const char * Get()
  {
    return "itkImageDomainFixedInterface";
  }
};

template< int D, class TPixel >
struct InterfaceName< itkImageMovingInterface< D, TPixel >>
{
  static const char * Get()
  {
    return "itkImageMovingInterface";
  }
};

template< int D, class TPixel >
struct InterfaceName< DisplacementFieldItkImageSourceInterface< D, TPixel >>
{
  static const char * Get()
  {
    return "DisplacementFieldItkImageSourceInterface";
  }
};

template< int D, class TPixel >
struct InterfaceName< itkMeshInterface< D, TPixel >>
{
  static const char * Get()
  {
    return "itkMeshInterface";
  }
};

template< >
struct InterfaceName< SourceInterface >
{
  static const char * Get()
  {
    return "SourceInterface";
  }
};

template< >
struct InterfaceName< SinkInterface >
{
  static const char * Get()
  {
    return "SinkInterface";
  }
};

template< int D, class TPixel >
struct InterfaceName< itkMetricv4Interface< D, TPixel >>
{
  static const char * Get()
  {
    return "itkMetricv4Interface";
  }
};

template< class InternalComputationValueType >
struct InterfaceName< itkOptimizerv4Interface< InternalComputationValueType >>
{
  static const char * Get()
  {
    return "itkOptimizerv4Interface";
  }
};

template< class InternalComputationValueType, int D >
struct InterfaceName< itkTransformInterface< InternalComputationValueType, D >>
{
  static const char * Get()
  {
    return "itkTransformInterface";
  }
};

template< class F, class M >
struct InterfaceName< elastixTransformParameterObjectInterface< F, M >>
{
  static const char * Get()
  {
    return "elastixTransformParameterObjectInterface";
  }
};

template< >
struct InterfaceName< RegistrationControllerStartInterface >
{
  static const char * Get()
  {
    return "RegistrationControllerStartInterface";
  }
};

template< >
struct InterfaceName< RunRegistrationInterface >
{
  static const char * Get()
  {
    return "RunRegistrationInterface";
  }
};

template< >
struct InterfaceName< AfterRegistrationInterface >
{
  static const char * Get()
  {
    return "AfterRegistrationInterface";
  }
};

template< >
struct InterfaceName< ReconnectTransformInterface >
{
  static const char * Get()
  {
    return "ReconnectTransformInterface";
  }
};

template< class InternalComputationValueType, int D >
struct InterfaceName< itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const char * Get()
  {
    return "itkTransformParametersAdaptorsContainerInterface";
  }
};

template< class InternalComputationValueType, int D >
struct InterfaceName< itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface< InternalComputationValueType, D >>
{
  static const char * Get()
  {
    return "itkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerInterface";
  }
};

// partial specialization of InterfaceName
// InterfaceName<T>::Get() should return the same name no matter whether T is an acceptor or provider interface.
template< typename InterfaceType >
struct InterfaceName< InterfaceAcceptor< InterfaceType >>
{
  static const char * Get()
  {
    return InterfaceName< InterfaceType >::Get();
  }
};

//************experimental**********

template< typename T >
struct Properties
{
  //static_assert(StaticErrorMessageRevealT<T>::False, "Please Implement InterfaceProperties<InterfaceType> for this InterfaceType");
  static const std::map<std::string, std::string> Get()
  {
    return{ { "NameOfInterface", InterfaceName< T >::Get() } };
  }
};

// The specializations for each type of Interface supported by the toolbox

template< int D, class TPixel >
struct Properties< itkImageFixedInterface< D, TPixel >>
{
  static const std::map<std::string, std::string> Get()
  {
    return{ { "NameOfInterface", InterfaceName< itkImageFixedInterface< D, TPixel> >::Get() }, { "Dimensionality", "3" }, { "PixelType", "float" } };
  }
};

template< >
struct Properties< MetricValueInterface >
{
  static const std::map<std::string, std::string> Get()
  {
    return{ { "NameOfInterface", "MetricValueInterface" } };
  }
};
} // end namespace selx
#endif // #define InterfaceTraits_h
