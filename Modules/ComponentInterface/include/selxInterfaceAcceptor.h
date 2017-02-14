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

#ifndef InterfaceAcceptor_h
#define InterfaceAcceptor_h

#include "selxComponentBase.h"

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
  using Pointer = std::shared_ptr<InterfaceAcceptor<InterfaceT>>;
  // Set() is called by a succesfull Connect()
  // The implementation of Set() must be provided by component developers.
  virtual int Set( typename InterfaceT::Pointer ) = 0;

  // Connect tries to connect this accepting interface with all interfaces of the provider component.
  int Connect( ComponentBase::Pointer );

  bool CanAcceptConnectionFrom( ComponentBase::ConstPointer );

private:

  bool isSet;
};

} //end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxInterfaceAcceptor.hxx"
#endif

#endif // InterfaceAcceptor_h