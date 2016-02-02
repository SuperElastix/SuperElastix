#ifndef ComponentBase_h
#define ComponentBase_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkMacro.h"

//#include "itkComponentBase.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
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

    typedef std::string                                                ParameterKeyType;
    typedef std::vector< std::string >                                 ParameterValueType;
    //typedef std::map< ParameterKeyType, ParameterValueType >           ParameterMapType;
    //TODO choose uniform naming for Typedefs
    typedef std::map<ParameterKeyType, ParameterValueType> CriteriaType;
    typedef std::pair<ParameterKeyType, ParameterValueType> CriterionType;

    enum interfaceStatus { success, noaccepter, noprovider };

    virtual interfaceStatus AcceptConnectionFrom(const char *, ComponentBase*) = 0;
    virtual int AcceptConnectionFrom(ComponentBase*) = 0;

    /** if there is any failed criterion, return false (like a short - circuit AND) */
    bool MeetsCriteria(const CriteriaType &criteria);

    virtual bool MeetsCriterion(const CriterionType &criterion) = 0;
  protected:
    virtual bool HasAcceptingInterface(const char *) = 0;
    virtual bool HasProvidingInterface(const char *) = 0;
    ComponentBase() {};
    virtual ~ComponentBase() {};

  };

} // end namespace selx


#endif // ComponentBase_h
