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

// All SuperElastix Components inherit from their interfaces classes. The interface classes as defined in 
// "selxInterfaces.h" are by default Providing. The InterfaceAcceptor class turns a Providing interface
// into an Accepting interface. For a SuperElastixComponent this differentiation is done by grouping the 
// interfaces either in Providing<Interfaces...> or in Accepting<Interfaces...>
template< class InterfaceT >
class InterfaceAcceptor
{
public:

  // Set() is called by a succesfull Connect()
  // The implementation of Set() must be provided by component developers.
  virtual int Set( InterfaceT * ) = 0;

  // Connect tries to connect this accepting interface with all interfaces of the provider component.
  int Connect( ComponentBase * );

  bool CanAcceptConnectionFrom(ComponentBase *);

private:

  bool isSet;
};


template <typename ... Interfaces>
class Accepting;

template <>
class Accepting<>
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType) { return 0; }
  int ConnectFromImpl(ComponentBase* other, const ComponentBase::InterfaceCriteriaType interfaceCriteria) { return 0; } //no interface called interfacename ;
  InterfaceStatus CanAcceptConnectionFrom(ComponentBase* other, const ComponentBase::InterfaceCriteriaType interfaceCriteria) { return InterfaceStatus::noaccepter; }
  int ConnectFromImpl(ComponentBase *) { return 0; }                                                                                   //Empty RestInterfaces does 0 successful connects ;

protected:


};

template< typename FirstInterface, typename ... RestInterfaces >
class Accepting< FirstInterface, RestInterfaces ... > : public InterfaceAcceptor< FirstInterface >, public Accepting< RestInterfaces ... >
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType);
  int ConnectFromImpl(ComponentBase* other, const ComponentBase::InterfaceCriteriaType interfaceCriteria);
  InterfaceStatus CanAcceptConnectionFrom(ComponentBase* other, const ComponentBase::InterfaceCriteriaType interfaceCriteria);
  int ConnectFromImpl( ComponentBase * );

protected:

};

template <typename ... Interfaces>
class Providing;

template< >
class Providing<>
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType) { return 0; }
protected:
};

template< typename FirstInterface, typename ... RestInterfaces >
class Providing< FirstInterface, RestInterfaces ... > : public FirstInterface, public Providing< RestInterfaces ... >
{
public:
  static unsigned int CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType);
protected:
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
  
  virtual int AcceptConnectionFrom(ComponentBase * other, const InterfaceCriteriaType interfaceCriteria);

  virtual int AcceptConnectionFrom( ComponentBase * );

protected:

  virtual InterfaceStatus CanAcceptConnectionFrom(ComponentBase* other, const InterfaceCriteriaType interfaceCriteria) override;

  virtual unsigned int CountAcceptingInterfaces(const ComponentBase::InterfaceCriteriaType interfaceCriteria){ return AcceptingInterfaces::CountMeetsCriteria(interfaceCriteria); };
  virtual unsigned int CountProvidingInterfaces(const ComponentBase::InterfaceCriteriaType interfaceCriteria){ return ProvidingInterfaces::CountMeetsCriteria(interfaceCriteria); };
  
};
} // end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixComponent.hxx"
#endif

#endif // #define SuperElastixComponent_h
