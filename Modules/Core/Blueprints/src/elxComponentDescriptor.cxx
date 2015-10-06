#ifndef __ComponentDescriptor_cxx
#define __ComponentDescriptor_cxx

#include "elxComponentDescriptor.h"

// TODO: Need enum for component name?

namespace elx {

ComponentDescriptor
::ComponentDescriptor( const ComponentNameType componentName )
{
  this->SetComponentName( componentName );
}

} // namespace elx

#endif // __ComponentDescriptor_cxx