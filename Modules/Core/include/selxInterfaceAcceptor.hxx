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

#ifndef InterfaceAcceptor_hxx
#define InterfaceAcceptor_hxx

namespace selx
{
template< class InterfaceT >
int
InterfaceAcceptor< InterfaceT >::Connect( ComponentBase::Pointer providerComponent )
{
  // Here the core of the handshake mechanism takes place: One specific 
  // interface of the Providing Component is taken (by dynamic_cast) and 
  // passed to the accepting Component.
  // The function returns the number of successful connects (1 or 0)

  std::shared_ptr< InterfaceT > providerInterface = std::dynamic_pointer_cast< InterfaceT >( providerComponent );
  if( !providerInterface )
  {
    // casting failed: this Providing Component does not have the specific interface. 
    return 0;
  }
  // connect value interfaces
  this->Accept( providerInterface ); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible at component level
  this->m_isSet = true;

  // TODO: see if we can get rid of all (dynamic) casts below.
  auto providerConnectionInfo = std::dynamic_pointer_cast<ConnectionInfo< InterfaceT >>( providerComponent);
  if (!providerConnectionInfo) // by definition should not fail
  {
    throw std::runtime_error( "std::dynamic_pointer_cast<ConnectionInfo< InterfaceT > should not fail by definition " );
  }

  ComponentBase* AcceptorBaseComponent = dynamic_cast<ComponentBase*>( this );
  if (!AcceptorBaseComponent) // by definition should not fail
  {
    throw std::runtime_error("dynamic_cast<ComponentBase*> should not fail by definition ");
  }

  providerConnectionInfo->SetProvidedTo(AcceptorBaseComponent->m_Name);
  return 1;
}


template< class InterfaceT >
bool
InterfaceAcceptor< InterfaceT >::CanAcceptConnectionFrom( ComponentBase::ConstPointer providerComponent )
{
  std::shared_ptr< const InterfaceT > providerInterface = std::dynamic_pointer_cast< const InterfaceT >( providerComponent );
  return bool(providerInterface);
}
} //end namespace selx
#endif // InterfaceAcceptor_hxx
