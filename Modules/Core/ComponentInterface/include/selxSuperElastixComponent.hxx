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

#ifndef selxSuperElastixComponent_hxx
#define selxSuperElastixComponent_hxx

#include "selxInterfaceTraits.h"
namespace selx
{
template< class InterfaceT >
int
InterfaceAcceptor< InterfaceT >::Connect( ComponentBase * providerComponent )
{
  InterfaceT * providerInterface = dynamic_cast< InterfaceT * >( providerComponent );
  if( !providerInterface )
  {
    //TODO log message?
    //std::cout << "providerComponent does not have required " << InterfaceName < InterfaceT >::Get() << std::endl;
    return 0;
  }
  // connect value interfaces
  this->Set( providerInterface ); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible at component level
  return 1;
}

template< class InterfaceT >
bool
InterfaceAcceptor< InterfaceT >::CanAcceptConnectionFrom(ComponentBase * providerComponent)
{
  InterfaceT * providerInterface = dynamic_cast< InterfaceT * >(providerComponent);
  return bool(providerInterface);
}

//////////////////////////////////////////////////////////////////////////
template< typename AcceptingInterfaces, typename ProvidingInterfaces >
int
SuperElastixComponent< AcceptingInterfaces, ProvidingInterfaces >::AcceptConnectionFrom(ComponentBase * other, const InterfaceCriteriaType interfaceCriteria)
{
  return AcceptingInterfaces::ConnectFromImpl(other, interfaceCriteria);
}


template< typename AcceptingInterfaces, typename ProvidingInterfaces >
int
SuperElastixComponent< AcceptingInterfaces, ProvidingInterfaces >::AcceptConnectionFrom( ComponentBase * other )
{
  return AcceptingInterfaces::ConnectFromImpl( other );
}

template< typename AcceptingInterfaces, typename ProvidingInterfaces >
InterfaceStatus
SuperElastixComponent< AcceptingInterfaces, ProvidingInterfaces >
::CanAcceptConnectionFrom(ComponentBase* other, const InterfaceCriteriaType interfaceCriteria)
{
  return AcceptingInterfaces::CanAcceptConnectionFrom(other, interfaceCriteria);
}

//////////////////////////////////////////////////////////////////////////
template< typename FirstInterface, typename ... RestInterfaces >
int
Accepting< FirstInterface, RestInterfaces ... >::ConnectFromImpl(ComponentBase * other, const ComponentBase::InterfaceCriteriaType interfaceCriteria)
{
  // Does our component have an accepting interface sufficing the right criteria (e.g interfaceName)?
  if (Count<FirstInterface>::MeetsCriteria(interfaceCriteria) == 1)   // We use the FirstInterface only (of each recursion level), thus the count can be 0 or 1
  {
    // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
    InterfaceAcceptor< FirstInterface > * acceptIF = this;
    // Make the connection to the other component by this interface and add the number of successes.
    return acceptIF->Connect(other) + Accepting< RestInterfaces ... >::ConnectFromImpl(other, interfaceCriteria);
  }
  else
  {
    return Accepting< RestInterfaces ... >::ConnectFromImpl(other, interfaceCriteria);
  }
}


template< typename FirstInterface, typename ... RestInterfaces >
int
Accepting< FirstInterface, RestInterfaces ... >::ConnectFromImpl( ComponentBase * other )
{
  // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
  InterfaceAcceptor< FirstInterface > * acceptIF = ( this );

  // See if the other component has the right interface and try to connect them
  // count the number of successes
  return acceptIF->Connect( other ) + Accepting< RestInterfaces ... >::ConnectFromImpl( other );
}


template< typename FirstInterface, typename ... RestInterfaces >
InterfaceStatus
Accepting< FirstInterface, RestInterfaces ... >::CanAcceptConnectionFrom(ComponentBase* other, const ComponentBase::InterfaceCriteriaType interfaceCriteria)
{

  InterfaceStatus restInterfacesStatus = Accepting< RestInterfaces ... >::CanAcceptConnectionFrom(other, interfaceCriteria);
  // if multiple interfaces were a success we do not have to check any further interfaces.
  if (restInterfacesStatus == InterfaceStatus::multiple)
  {
    return InterfaceStatus::multiple;
  }
  // We use the FirstInterface only (of each recursion level), thus the count can be 0 or 1
  unsigned int interfaceMeetsCriteria = Count<FirstInterface>::MeetsCriteria(interfaceCriteria);
  if (interfaceMeetsCriteria == 0) // InterfaceStatus::noaccepter;
  {
    // no new success, keep the status of previous recursion
    return restInterfacesStatus;
  }
  else // This "FirstInterface" of the component is an acceptor interface that fulfills the criteria
  {
    InterfaceAcceptor< FirstInterface > * acceptIF = (this);
    if (acceptIF->CanAcceptConnectionFrom(other))
    {
      // if a previous interface was a success, we can have either success or multiple (successes)
      if (restInterfacesStatus == InterfaceStatus::success)
        return InterfaceStatus::multiple;
      return InterfaceStatus::success;
    }
    else // InterfaceStatus::noprovider
    {
      // The found acceptor interface is not identical to a providing interface of the other component
      if (restInterfacesStatus == InterfaceStatus::noaccepter)
        return InterfaceStatus::noprovider;
      return restInterfacesStatus;
    }
  }
  // never reached
  return InterfaceStatus::noaccepter;
}


template< typename FirstInterface, typename ... RestInterfaces >
unsigned int
Accepting< FirstInterface, RestInterfaces ... >::CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType interfaceCriteria)
{
  return Count<FirstInterface, RestInterfaces ... >::MeetsCriteria(interfaceCriteria);
}

template< typename FirstInterface, typename ... RestInterfaces >
unsigned int
Providing< FirstInterface, RestInterfaces ... >::CountMeetsCriteria(const ComponentBase::InterfaceCriteriaType interfaceCriteria)
{
  return Count<FirstInterface, RestInterfaces ... >::MeetsCriteria(interfaceCriteria);
}


template <typename FirstInterface, typename ... RestInterfaces>
unsigned int Count<FirstInterface, RestInterfaces ...>::MeetsCriteria(const ComponentBase::InterfaceCriteriaType interfaceCriteria)
{
  auto interfaceProperies = Properties< FirstInterface >::Get();
  for (const auto keyAndValue : interfaceCriteria)
  {
    auto && key = keyAndValue.first;
    auto && value = keyAndValue.second;
    if (interfaceProperies.count(key) != 1 || interfaceProperies[key].compare(value)!=0 )
    {
      // as soon as any of the criteria fails we break the loop and test the RestInterfaces
      return Count< RestInterfaces ... >::MeetsCriteria(interfaceCriteria);
    }
  }
  // if all criteria are met for this Interface we add 1 to the count and continue with the RestInterfaces
  return 1 + Count< RestInterfaces ... >::MeetsCriteria(interfaceCriteria);
};

} // end namespace selx

#endif // #define selxSuperElastixComponent_hxx
