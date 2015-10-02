#ifndef __ComponentDescriptor_hxx
#define __ComponentDescriptor_hxx

#include "elxComponentDescriptor.h"

// TODO: Need enum for component name?

namespace elx {

ComponentDescriptor
::ComponentDescriptor( const ComponentNameType componentName )
{
  this->SetComponentName( componentName );
  this->SetComponentUniqueId( this->GenerateUniqueId() );
  this->SetParameterMap( this->ParameterMapType() );
}

ComponentDescriptor
::ComponentDescriptor( const ComponentNameType componentName, 
                       const ComponentUniqueIdType componentUniqueId )
{
  this->SetComponentName( componentName );
  this->SetComponentUniqueId( componentUniqueId );
  this->SetParameterMap( this->ParameterMapType() );
}

ComponentDescriptor
::ComponentDescriptor( const ComponentNameType componentName,
                       const ComponentIdType componentUniqueId,
                       const ParameterMapType parameterMap )
{
  this->SetComponentName( componentName );
  this->SetComponentUniqueId( componentUniqueId );
  this->SetParameterMap( parameterMap );
}

ComponentDescriptor::ComponentIdType
ComponentDescriptor
::GenerateUniqueId()
{

}

} // namespace elx

#endif // __ComponentDescriptor_hxx