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
} // end namespace selx
#endif // #define InterfaceTraits_h
