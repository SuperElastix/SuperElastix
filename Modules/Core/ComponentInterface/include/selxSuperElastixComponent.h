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

#ifndef SuperElastixComponent_h
#define SuperElastixComponent_h

#include "selxComponentBase.h"
#include <typeinfo>
#include <string>

namespace selx
{
// Define the accepting interfaces as templated by the providing interface

template< class InterfaceT >
class InterfaceAcceptor
{
public:

  // Set() is called by a succesfull Connect()
  // The implementation of Set() must be provided by component developers.
  virtual int Set( InterfaceT * ) = 0;

  // Connect tries to connect this accepting interface with all interfaces of the provider component.
  int Connect( ComponentBase * );

private:

  bool isSet;
};

template< typename ... RestInterfaces >
class Accepting
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType) { return 0; }
  ComponentBase::interfaceStatus ConnectFromImpl( const char *, ComponentBase * ) { return ComponentBase::interfaceStatus::noaccepter; } //no interface called interfacename ;
  int ConnectFromImpl( ComponentBase * ) { return 0; }                                                                                   //Empty RestInterfaces does 0 successful connects ;

protected:

  bool HasInterface( const char * ) { return false; }
};

template< typename FirstInterface, typename ... RestInterfaces >
class Accepting< FirstInterface, RestInterfaces ... > : public InterfaceAcceptor< FirstInterface >, public Accepting< RestInterfaces ... >
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType);
  ComponentBase::interfaceStatus ConnectFromImpl( const char *, ComponentBase * );

  int ConnectFromImpl( ComponentBase * );

protected:

  bool HasInterface( const char * );
};

template< typename ... RestInterfaces >
class Providing
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType) { return 0; }
protected:

  bool HasInterface( const char * ) { return false; }
};

template< typename FirstInterface, typename ... RestInterfaces >
class Providing< FirstInterface, RestInterfaces ... > : public FirstInterface, public Providing< RestInterfaces ... >
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType);
protected:

  bool HasInterface( const char * );
};

//template<typename... Interfaces>
//class Providing : public Interfaces...
//{
//};

// helper class for SuperElastixComponent::CountAcceptingInterfaces and SuperElastixComponent::CountProvidingInterfaces to loop over a set of interfaces

template <typename ... Interfaces>
struct Count;

template <>
struct Count<>
{
  static unsigned int MeetsCriteria(const ComponentBase::InterfaceCriteriaType) { return 0; };
};

template < typename FirstInterface, typename ... RestInterfaces>
struct Count< FirstInterface, RestInterfaces ... >
{
  static unsigned int MeetsCriteria(const ComponentBase::InterfaceCriteriaType);
};

template< typename AcceptingInterfaces, typename ProvidingInterfaces >
class SuperElastixComponent : public AcceptingInterfaces, public ProvidingInterfaces, public ComponentBase
{
public:
  
  using AcceptingInterfacesTypeList = AcceptingInterfaces;
  using ProvidingInterfacesTypeList = ProvidingInterfaces;
  
  virtual interfaceStatus AcceptConnectionFrom( const char *, ComponentBase * );

  virtual int AcceptConnectionFrom( ComponentBase * );

protected:

  virtual bool HasAcceptingInterface( const char * );

  virtual bool HasProvidingInterface( const char * );

  //experimental
  //SuperElastixComponentType::AcceptingInterfacesTypeList::CountMeetsCriteria(InterfaceCriteriaType);
  //virtual unsigned int CountAcceptingInterfaces(const ComponentBase::InterfaceCriteriaType);
  //virtual unsigned int CountProvidingInterfaces(const ComponentBase::InterfaceCriteriaType);
  
};
} // end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixComponent.hxx"
#endif

#endif // #define SuperElastixComponent_h
