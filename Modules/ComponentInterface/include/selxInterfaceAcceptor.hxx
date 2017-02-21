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
  std::shared_ptr< InterfaceT > providerInterface = std::dynamic_pointer_cast< InterfaceT >( providerComponent );
  if( !providerInterface )
  {
    //TODO log message?
    //std::cout << "providerComponent does not have required " << InterfaceName < InterfaceT >::Get() << std::endl;
    return 0;
  }
  // connect value interfaces
  this->Set( providerInterface ); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible at component level
  this->isSet = true;
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
