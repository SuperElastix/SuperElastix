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

namespace selx
{

//////////////////////////////////////////////////////////////////////////
template< typename AcceptingInterfaces, typename ProvidingInterfaces >
int
SuperElastixComponent< AcceptingInterfaces, ProvidingInterfaces >::AcceptConnectionFrom( ComponentBase * other,
  const InterfaceCriteriaType interfaceCriteria )
{
  return AcceptingInterfaces::ConnectFromImpl( other, interfaceCriteria );
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
::CanAcceptConnectionFrom( ComponentBase * other, const InterfaceCriteriaType interfaceCriteria )
{
  return AcceptingInterfaces::CanAcceptConnectionFrom( other, interfaceCriteria );
}


} // end namespace selx

#endif // #define selxSuperElastixComponent_hxx
