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

#ifndef Accepting_h
#define Accepting_h

namespace selx
{
template< typename ... Interfaces >
class Accepting;

template< >
class Accepting< >
{
public:

  static unsigned int CountMeetsCriteria( const ComponentBase::InterfaceCriteriaType ) { return 0; }
  int ConnectFromImpl( ComponentBase * other, const ComponentBase::InterfaceCriteriaType interfaceCriteria ) { return 0; } //no interface called interfacename ;
  InterfaceStatus CanAcceptConnectionFrom( ComponentBase * other, const ComponentBase::InterfaceCriteriaType interfaceCriteria )
  {
    return InterfaceStatus::noaccepter;
  }
  int ConnectFromImpl( ComponentBase * ) { return 0; }                                                                                   //Empty RestInterfaces does 0 successful connects ;

protected:
};

template< typename FirstInterface, typename ... RestInterfaces >
class Accepting< FirstInterface, RestInterfaces ... > : public InterfaceAcceptor< FirstInterface >, public Accepting< RestInterfaces ... >
{
public:

  static unsigned int CountMeetsCriteria( const ComponentBase::InterfaceCriteriaType );

  int ConnectFromImpl( ComponentBase * other, const ComponentBase::InterfaceCriteriaType interfaceCriteria );

  InterfaceStatus CanAcceptConnectionFrom( ComponentBase * other, const ComponentBase::InterfaceCriteriaType interfaceCriteria );

  int ConnectFromImpl( ComponentBase * );

protected:
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxAccepting.hxx"
#endif
#endif // Accepting_h