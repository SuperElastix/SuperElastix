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

#include "Interfaces.h"
#include "itkMacro.h"
//#include <boost/static_assert.hpp>
#include <boost/mpl/assert.hpp>
//#include <type_traits>

namespace selx
{
// Traits to get printable interface name

// In our toolbox for each InterfaceType it is required to implement InterfaceName<InterfaceType>::Get()
// If one omits this, the implementation falls back to the default implementation that provides a compiler error message including the InterfaceType name for which the specialization is missing.
template <typename T>
struct InterfaceName
{
  struct PLEASE_IMPLEMENT_INTERFACENAME_GET_FOR_THIS_TYPE; 
  //TODO message produced by BOOST_MPL_ASSERT_MSG is not yet very clear
  BOOST_MPL_ASSERT_MSG( false, PLEASE_IMPLEMENT_INTERFACENAME_GET_FOR_THIS_TYPE, (T) );
  static const char* Get()
  { 
    //static_assert(false, "Please implement a template specialization for the appropriate InterfaceName");
    // Exception should never happen since build should fail due to static assert of BOOST_MPL_ASSERT_MSG.
    itkGenericExceptionMacro("Please implement a template specialization for the appropriate InterfaceName");
    return typeid(T).name();
  }
};

// The specializations for each type of Interface supported by the toolbox

template <>
struct InterfaceName < MetricValueInterface >
{
  static const char* Get()
  {
    return "MetricValueInterface";
  }
};
template <>
struct InterfaceName < MetricDerivativeInterface >
{
  static const char* Get()
  {
    return "MetricDerivativeInterface";
  }
};
template <>
struct InterfaceName < OptimizerUpdateInterface >
{
  static const char* Get()
  {
    return "OptimizerUpdateInterface";
  }
};

template <>
struct InterfaceName < ConflictinUpdateInterface >
{
  static const char* Get()
  {
    return "ConflictinUpdateInterface";
  }
};

template <>
struct InterfaceName < TransformedImageInterface >
{
  static const char* Get()
  {
    return "TransformedImageInterface";
  }
};


// InterfaceName<T>::Get() should return "itkImageSourceInterface" no matter over which arguments itkImageSourceInterface is templated
template <int D, class TPixel>
struct InterfaceName < itkImageInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < itkImageFixedInterface <D, TPixel> >
{
	static const char* Get()
	{
		return "itkImageFixedInterface";
	}
};

template <int D, class TPixel>
struct InterfaceName < itkImageMovingInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageMovingInterface";
  }
};


template <int D, class TPixel>
struct InterfaceName < DisplacementFieldItkImageSourceInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "DisplacementFieldItkImageSourceInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < itkMeshInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkMeshInterface";
  }
};


template <>
struct InterfaceName < SourceInterface >
{
  static const char* Get()
  {
    return "SourceInterface";
  }
};

template <>
struct InterfaceName < SinkInterface >
{
  static const char* Get()
  {
    return "SinkInterface";
  }
};


template <int D, class TPixel>
struct InterfaceName < itkMetricv4Interface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkMetricv4Interface";
  }
};

template <class InternalComputationValueType>
struct InterfaceName < itkOptimizerv4Interface <InternalComputationValueType> >
{
  static const char* Get()
  {
    return "itkOptimizerv4Interface";
  }
};

template <class InternalComputationValueType, int D>
struct InterfaceName < itkTransformInterface <InternalComputationValueType, D> >
{
  static const char* Get()
  {
    return "itkTransformInterface";
  }
};


template <>
struct InterfaceName < RunRegistrationInterface >
{
  static const char* Get()
  {
    return "RunRegistrationInterface";
  }
};

template <>
struct InterfaceName < AfterRegistrationInterface >
{
  static const char* Get()
  {
    return "AfterRegistrationInterface";
  }
};

// partial specialization of InterfaceName
// InterfaceName<T>::Get() should return the same name no matter whether T is an acceptor or provider interface.
template<typename T1>
struct InterfaceName < InterfaceAcceptor<T1> > {
  static const char* Get()
  {
    return InterfaceName<T1>::Get();
  }
};

} // end namespace selx


#endif // #define InterfaceTraits_h
