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

#ifndef Interfaces_hxx
#define Interfaces_hxx

#include "selxInterfaceTraits.h"
namespace selx
{
template<class InterfaceT>
int InterfaceAcceptor<InterfaceT>::Connect(ComponentBase* providerComponent){

  InterfaceT* providerInterface = dynamic_cast<InterfaceT*> (providerComponent);
  if (!providerInterface)
  {
    //TODO log message?
    //std::cout << "providerComponent does not have required " << InterfaceName < InterfaceT >::Get() << std::endl;
    return 0;
  }
  // connect value interfaces
  this->Set(providerInterface); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible at component level
  return 1;
}
//////////////////////////////////////////////////////////////////////////
template<typename AcceptingInterfaces, typename ProvidingInterfaces>
ComponentBase::interfaceStatus Implements<AcceptingInterfaces, ProvidingInterfaces>::AcceptConnectionFrom(const char * interfacename, ComponentBase* other)
{
  return AcceptingInterfaces::ConnectFromImpl(interfacename, other);
}

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
int Implements<AcceptingInterfaces, ProvidingInterfaces>::AcceptConnectionFrom(ComponentBase* other)
{
  return AcceptingInterfaces::ConnectFromImpl(other);
}

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
bool Implements<AcceptingInterfaces, ProvidingInterfaces>::HasAcceptingInterface(const char * interfacename)
{
  return AcceptingInterfaces::HasInterface(interfacename);
}

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
bool Implements<AcceptingInterfaces, ProvidingInterfaces>::HasProvidingInterface(const char * interfacename)
{
  return ProvidingInterfaces::HasInterface(interfacename);
}

//////////////////////////////////////////////////////////////////////////
template<typename FirstInterface, typename ... RestInterfaces>
ComponentBase::interfaceStatus Accepting<FirstInterface, RestInterfaces... >::ConnectFromImpl(const char * interfacename, ComponentBase* other)
{
  // does our component have an accepting interface called interfacename? 
  if (InterfaceName<InterfaceAcceptor<FirstInterface>>::Get() == std::string(interfacename))
  {
    // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
    InterfaceAcceptor<FirstInterface>* acceptIF = this;

    // See if the other component has the right interface and try to connect them
    if (1 == acceptIF->Connect(other))
    {
      //success. By terminating this function, we assume only one interface listens to interfacename and that one connection with the other component can be made by this name
      return ComponentBase::interfaceStatus::success;
    }
    else
    {
      // interfacename was found, but other component doesn't match
      return ComponentBase::interfaceStatus::noprovider;
    }
  }
  return Accepting< RestInterfaces ... >::ConnectFromImpl(interfacename, other);
}

template<typename FirstInterface, typename ... RestInterfaces>
int Accepting<FirstInterface, RestInterfaces... >::ConnectFromImpl(ComponentBase* other)
{
  // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
  InterfaceAcceptor<FirstInterface>* acceptIF = (this);

  // See if the other component has the right interface and try to connect them
  // count the number of successes
  return acceptIF->Connect(other) + Accepting< RestInterfaces ... >::ConnectFromImpl(other);
}

template<typename FirstInterface, typename ... RestInterfaces>
bool Accepting<FirstInterface, RestInterfaces... >::HasInterface(const char* interfacename)
{
  if (InterfaceName<InterfaceAcceptor<FirstInterface>>::Get() == std::string(interfacename))
  {
    return true;
  }
  return Accepting< RestInterfaces ... >::HasInterface(interfacename);
}

template<typename FirstInterface, typename ... RestInterfaces>
bool Providing<FirstInterface, RestInterfaces... >::HasInterface(const char* interfacename)
{
  if (InterfaceName<FirstInterface>::Get() == std::string( interfacename))
  {
    return true;
  }
  return Providing< RestInterfaces ... >::HasInterface(interfacename);
}

} // end namespace selx


#endif // #define Interfaces_hxx