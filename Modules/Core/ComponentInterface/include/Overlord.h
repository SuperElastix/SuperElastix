#ifndef Overlord_h
#define Overlord_h

#include "itkLightProcessObject.h"

#include "itkObjectFactory.h"
//#include "itkComponentBase.h"
#include <list>
#include <vector>
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
    typedef std::pair<std::string, std::string> CriterionType;

    typedef ComponentBase       ComponentType;
    typedef ComponentSelector::Pointer ComponentSelectorPointer;
    typedef std::vector<ComponentSelectorPointer> ComponentSelectorContainerType;
    typedef ComponentSelectorContainerType::iterator ComponentSelectorIteratorType;

    void SetBlueprint(const Blueprint::Pointer);
    bool Configure();
  protected:
    
    Overlord() {};
    virtual ~Overlord() {};
  private:
    void ApplyNodeConfiguration();
    void ApplyConnectionConfiguration();
    bool UpdateSelectors();
    bool ConnectComponents();
    Blueprint::Pointer m_Blueprint;
    ComponentSelectorContainerType m_ComponentSelectorContainer;
  };

} // end namespace selx


#endif // Overlord_h
