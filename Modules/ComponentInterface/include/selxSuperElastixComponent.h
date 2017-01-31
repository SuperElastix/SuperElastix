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

#include "selxMacro.h"
#include "selxComponentBase.h"
#include <typeinfo>
#include <string>
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"
#include "selxInterfaceAcceptor.h"
#include "selxAccepting.h"
#include "selxProviding.h"
#include "selxCount.h"


namespace selx
{

template< typename AcceptingInterfaces, typename ProvidingInterfaces >
class SuperElastixComponent : public AcceptingInterfaces, public ProvidingInterfaces, public ComponentBase
{
public:

  using AcceptingInterfacesTypeList = AcceptingInterfaces;
  using ProvidingInterfacesTypeList = ProvidingInterfaces;

  virtual int AcceptConnectionFrom( ComponentBase * other, const InterfaceCriteriaType interfaceCriteria );

  virtual int AcceptConnectionFrom( ComponentBase * );

protected:

  virtual InterfaceStatus CanAcceptConnectionFrom( ComponentBase * other, const InterfaceCriteriaType interfaceCriteria ) override;

  virtual unsigned int CountAcceptingInterfaces( const ComponentBase::InterfaceCriteriaType interfaceCriteria ) override
  {
    return AcceptingInterfaces::CountMeetsCriteria( interfaceCriteria );
  }
  virtual unsigned int CountProvidingInterfaces( const ComponentBase::InterfaceCriteriaType interfaceCriteria ) override
  {
    return ProvidingInterfaces::CountMeetsCriteria( interfaceCriteria );
  }
};
} // end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixComponent.hxx"
#endif

#endif // #define SuperElastixComponent_h
