#ifndef __ComponentDescriptor_h
#define __ComponentDescriptor_h

#include "elxMacro.h"
#include "itkLightObject.h"

namespace elx {

class ComponentDescriptor : public itk::LightObject
{
public:

  elxNewMacro( ComponentDescriptor, itk::LightObject );

  typedef std::string                                         ComponentNameType;
  typedef std::map< std::string, std::vector< std::string > > ParameterMapType;

  void SetComponentName( ComponentNameType componentName ) { this->m_ComponentName = componentName; };
  ComponentNameType GetComponentName( void ) { return this->m_ComponentName; };

private:

  ComponentNameType  m_ComponentName;
  ParameterMapType   m_ParameterMap;

};

}

#endif // __ComponentDescriptor_h