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

#ifndef Accepting_hxx
#define Accepting_hxx

#include "selxLogger.h"
#include "selxCount.h"

namespace selx
{

template< typename FirstInterface, typename ... RestInterfaces  >
Accepting< FirstInterface, RestInterfaces ... >::Accepting() : m_Logger(*(new LoggerImpl()))
{
  //TODO: remove this constructor as it should not be used
}


template< typename FirstInterface, typename ... RestInterfaces  >
Accepting< FirstInterface, RestInterfaces ... >::Accepting( LoggerImpl & logger ) : m_Logger(logger)
{}

template< typename FirstInterface, typename ... RestInterfaces >
int
Accepting< FirstInterface, RestInterfaces ... >::ConnectFromImpl( ComponentBase::Pointer other,
  const ComponentBase::InterfaceCriteriaType interfaceCriteria )
{
  // Does our component have an accepting interface sufficing the right criteria (e.g interfaceName)?
  if( Count< FirstInterface >::MeetsCriteria( interfaceCriteria ) == 1 )   // We use the FirstInterface only (of each recursion level), thus the count can be 0 or 1
  {
    // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
    InterfaceAcceptor< FirstInterface > * acceptIF = this;
    // Make the connection to the other component by this interface and add the number of successes.
    return acceptIF->Connect( other ) + Accepting< RestInterfaces ... >::ConnectFromImpl( other, interfaceCriteria );
  }
  else
  {
    return Accepting< RestInterfaces ... >::ConnectFromImpl( other, interfaceCriteria );
  }
}


template< typename FirstInterface, typename ... RestInterfaces >
int
Accepting< FirstInterface, RestInterfaces ... >::ConnectFromImpl( ComponentBase::Pointer other )
{
  // cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
  InterfaceAcceptor< FirstInterface > * acceptIF = ( this );

  // See if the other component has the right interface and try to connect them
  // count the number of successes
  return acceptIF->Connect( other ) + Accepting< RestInterfaces ... >::ConnectFromImpl( other );
}


template< typename FirstInterface, typename ... RestInterfaces >
InterfaceStatus
Accepting< FirstInterface, RestInterfaces ... >::CanAcceptConnectionFrom( ComponentBase::ConstPointer other,
  const ComponentBase::InterfaceCriteriaType interfaceCriteria )
{
  InterfaceStatus restInterfacesStatus = Accepting< RestInterfaces ... >::CanAcceptConnectionFrom( other, interfaceCriteria );
  // if multiple interfaces were a success we do not have to check any further interfaces.
  if( restInterfacesStatus == InterfaceStatus::multiple )
  {
    return InterfaceStatus::multiple;
  }
  // We use the FirstInterface only (of each recursion level), thus the count can be 0 or 1
  unsigned int interfaceMeetsCriteria = Count< FirstInterface >::MeetsCriteria( interfaceCriteria );
  if( interfaceMeetsCriteria == 0 ) // InterfaceStatus::noaccepter;
  {
    // no new success, keep the status of previous recursion
    return restInterfacesStatus;
  }
  else // This "FirstInterface" of the component is an acceptor interface that fulfills the criteria
  {
    InterfaceAcceptor< FirstInterface > * acceptIF = ( this );
    if( acceptIF->CanAcceptConnectionFrom( other ) )
    {
      // if a previous interface was a success, we can have either success or multiple (successes)
      if( restInterfacesStatus == InterfaceStatus::success )
      {
        return InterfaceStatus::multiple;
      }
      return InterfaceStatus::success;
    }
    else // InterfaceStatus::noprovider
    {
      // The found acceptor interface is not identical to a providing interface of the other component
      if( restInterfacesStatus == InterfaceStatus::noaccepter )
      {
        return InterfaceStatus::noprovider;
      }
      return restInterfacesStatus;
    }
  }
  // never reached
  return InterfaceStatus::noaccepter;
}


template< typename FirstInterface, typename ... RestInterfaces >
unsigned int
Accepting< FirstInterface, RestInterfaces ... >::CountMeetsCriteria( const ComponentBase::InterfaceCriteriaType interfaceCriteria )
{
  return Count< FirstInterface, RestInterfaces ... >::MeetsCriteria( interfaceCriteria );
}


template< typename FirstInterface, typename ... RestInterfaces >
bool
Accepting< FirstInterface, RestInterfaces ... >::AreAllAccepted()
{
  // The interface acceptor has stored the last accepted interface, which is empty if no connection was made.
  if (!this->InterfaceAcceptor< FirstInterface >::GetAccepted())
  {
    // TODO: Log component names as well
    this->m_Logger.Log( LogLevel::ERR, "No connection satisfy criteria {0}.", this->m_Logger.ToString(Properties< FirstInterface >::Get()));
    return false;
  }

  return Accepting< RestInterfaces ... >::AreAllAccepted();
}
} //end namespace selx
#endif // Accepting_hxx
