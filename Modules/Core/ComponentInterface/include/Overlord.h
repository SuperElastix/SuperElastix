#ifndef Overlord_h
#define Overlord_h

#include "itkLightProcessObject.h"

#include "itkObjectFactory.h"
//#include "itkComponentBase.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>

#include "elxMacro.h"
#include "elxBlueprint.h"
//#include "ComponentBase.h"
#include "ComponentFactory.h"

namespace selx
{

  class Overlord : public itk::LightProcessObject
  {
  public:

    elxNewMacro(Overlord, itk::LightProcessObject);

    typedef std::map<std::string, std::string> CriteriaType;
    typedef std::pair<std::string, std::string> CriteriumType;

    typedef ComponentBase       ComponentType;
    //typedef ComponentSelector::Pointer ComponentSelectorPointer;
    //typedef std::list<ComponentSelectorPointer> ComponentSelectorList;

    void SetBlueprint(const Blueprint::Pointer);
    int Update();
  protected:

    Overlord() {};
    virtual ~Overlord() {};
  private:
    Blueprint::Pointer m_Blueprint;
  };

} // end namespace selx


#endif // Overlord_h
