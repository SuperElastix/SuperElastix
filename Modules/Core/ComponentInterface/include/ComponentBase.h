#ifndef ComponentBase_h
#define ComponentBase_h

#include "itkLightObject.h"

#include "itkObjectFactory.h"
//#include "itkComponentBase.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace selx
{

  class ComponentBase : public itk::LightObject
  {
  public:
    /** Standard class typedefs */
    typedef ComponentBase   Self;
    typedef itk::LightObject        Superclass;
    typedef itk::SmartPointer< Self >      Pointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro(ComponentBase, Superclass);

    typedef std::map<std::string, std::string> CriteriaType;
    typedef std::pair<std::string, std::string> CriteriumType;

    enum interfaceStatus { success, noaccepter, noprovider };

    virtual interfaceStatus ConnectFrom(const char *, ComponentBase*) = 0;
    virtual int ConnectFrom(ComponentBase*) = 0;
    virtual bool MeetsCriteria(const CriteriaType &criteria) = 0;

  protected:
    ComponentBase() {};
    virtual ~ComponentBase() {};

  };

} // end namespace selx


#endif // ComponentBase_h
