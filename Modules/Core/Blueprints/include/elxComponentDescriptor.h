#ifndef __ComponentDescriptor_h
#define __ComponentDescriptor_h

#include "elxMacro.h"
#include "itkDataObject.h"

namespace elx {

class ComponentDescriptor : public itk::DataObject
{
public:

  elxNewMacro( ComponentDescriptor, itk::DataObject );

  // Identifier to find component in the component database
  typedef std::string ComponentNameType;

  // TODO: Setter should validate ComponentName exists in ComponentDatabase
  itkSetMacro( ComponentName, ComponentNameType ); 
  itkGetMacro( ComponentName, ComponentNameType );

private:

  ComponentNameType m_ComponentName;

};

}

#endif // __ComponentDescriptor_h
