/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef selxComponentSelector_hxx
#define selxComponentSelector_hxx

#include "selxComponentSelector.h"

namespace selx
{
template< >
struct ContructComponentsFromTypeList< TypeList< >>
{
  static std::list< ComponentBase::Pointer > fill( std::list< ComponentBase::Pointer > & components, const std::string & name )
  {
    return components;
  }
};

template< typename ComponentType, typename ... Rest >
struct ContructComponentsFromTypeList< TypeList< ComponentType, Rest ... >>
{
  static std::list< ComponentBase::Pointer > fill( std::list< ComponentBase::Pointer > & components, const std::string & name )
  {
    components.push_back( std::make_shared< ComponentType >( name ) );
    return ContructComponentsFromTypeList< TypeList< Rest ... >>::fill( components, name );
  }
};

template< class ComponentList >
ComponentSelector< ComponentList >::ComponentSelector( const std::string & name )
{
  m_PossibleComponents = std::list< ComponentBase::Pointer >();
  m_PossibleComponents = ContructComponentsFromTypeList< ComponentList >::fill( m_PossibleComponents, name );
}


template< class ComponentList >
void
ComponentSelector< ComponentList >::AddCriterion( const CriterionType & criterion )
{
  this->m_PossibleComponents.remove_if([ & ]( ComponentBasePointer component ){
      return !component->MeetsCriterion( criterion );
    } );
}


template< class ComponentList >
void
ComponentSelector< ComponentList >::AddAcceptingInterfaceCriteria( const InterfaceCriteriaType & interfaceCriteria )
{
  this->m_PossibleComponents.remove_if([ & ]( ComponentBasePointer component ){
      return 0 == component->CountAcceptingInterfaces( interfaceCriteria );
    } );
}


template< class ComponentList >
void
ComponentSelector< ComponentList >::AddProvidingInterfaceCriteria( const InterfaceCriteriaType & interfaceCriteria )
{
  this->m_PossibleComponents.remove_if([ & ]( ComponentBasePointer component ){
      return 0 == component->CountProvidingInterfaces( interfaceCriteria );
    } );
}


// CompatibleInterfaces
template< class ComponentList >
unsigned int
ComponentSelector< ComponentList >::RequireAcceptingInterfaceFrom( ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria )
{
  this->m_PossibleComponents.remove_if([ & ]( ComponentBasePointer component ){
      auto status = component->CanAcceptConnectionFrom( other, interfaceCriteria );
      return status == InterfaceStatus::noaccepter || status == InterfaceStatus::noprovider;
    } );
  return 0;
}


template< class ComponentList >
unsigned int
ComponentSelector< ComponentList >::RequireProvidingInterfaceTo( ComponentBasePointer other, const InterfaceCriteriaType & interfaceCriteria )
{
  this->m_PossibleComponents.remove_if([ & ]( ComponentBasePointer component ){
      auto status = other->CanAcceptConnectionFrom( component, interfaceCriteria );
      return status == InterfaceStatus::noaccepter || status == InterfaceStatus::noprovider;
    } );
  return 0;
}


template< class ComponentList >
typename ComponentSelector< ComponentList >::ComponentBasePointer
ComponentSelector< ComponentList >::GetComponent()
{
  //TODO check if Modified
  //this->UpdatePossibleComponents();

  if( this->m_PossibleComponents.size() == 1 )
  {
    return *( this->m_PossibleComponents.begin() );
  }
  else
  {
    return ITK_NULLPTR;
  }
}


template< class ComponentList >
unsigned int
ComponentSelector< ComponentList >::NumberOfComponents()
{
  return this->m_PossibleComponents.size();
}


template< class ComponentList >
void
ComponentSelector< ComponentList >::PrintComponents( void )
{
  /*
  for (auto & component : this->m_PossibleComponents)
  {
    auto const & properties = component->TemplateProperties();
    for (auto const & keyvalue : properties)
    {
      std::cout << "{ """ << keyvalue.first << """: """ << keyvalue.second << """}" << std::endl;
    }
    std::cout << std::endl;
  }
  */
}
} // end namespace selx

#endif //selxComponentSelector_hxx
