#ifndef Overlord_h
#define Overlord_h

#include "itkLightProcessObject.h"
#include "itkObjectFactory.h"
#include "itkVectorContainer.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>

#include "elxMacro.h"
#include "elxBlueprint.h"
#include "ComponentFactory.h"
#include "interfaces.h"
namespace selx
{

  class Overlord : public itk::LightProcessObject
  {
  public:

    elxNewMacro(Overlord, itk::LightProcessObject);

    typedef ComponentBase::CriteriaType CriteriaType;
    typedef ComponentBase::CriterionType CriterionType;
    typedef ComponentBase::ParameterValueType ParameterValueType;
    //typedef std::map<std::string, std::string> CriteriaType;
    //typedef std::pair<std::string, std::string> CriterionType;

    typedef ComponentBase       ComponentType;
    typedef ComponentSelector::Pointer ComponentSelectorPointer;
    typedef std::vector<ComponentSelectorPointer> ComponentSelectorContainerType;
    typedef ComponentSelectorContainerType::iterator ComponentSelectorIteratorType;

    typedef itk::ImageFileReader<itk::Image<float, 2>> Reader2floatType;
    typedef itk::ImageFileWriter<itk::Image<float, 2>> Writer2floatType;

    typedef itk::VectorContainer <
      unsigned int, Reader2floatType::Pointer > Reader2floatContainerType;

    typedef itk::VectorContainer <
      unsigned int, Writer2floatType::Pointer > Writer2floatContainerType;


    typedef itk::ImageFileReader<itk::Image<double, 3>> Reader3doubleType;
    typedef itk::ImageFileWriter<itk::Image<double, 3>> Writer3doubleType;

    typedef itk::VectorContainer <
      unsigned int, Reader3doubleType::Pointer > Reader3doubleContainerType;

    typedef itk::VectorContainer <
      unsigned int, Writer3doubleType::Pointer > Writer3doubleContainerType;


   // typedef itk::Object::Pointer ObjectPointer;
   // typedef itk::VectorContainer <
   //   unsigned int, ObjectPointer > ObjectContainerType;

    //typedef itk::VectorContainer <
    //  unsigned int, SinkInterface* > SinkComponentsContainerType;

    //typedef itk::VectorContainer <
    //  unsigned int, SourceInterface* > SourceComponentsContainerType;

    //TODO these containers are not interface specific
    typedef itk::VectorContainer <
      unsigned int, ComponentType::Pointer > SinkComponentsContainerType;

    typedef itk::VectorContainer <
      unsigned int, ComponentType::Pointer > SourceComponentsContainerType;

   // typedef std::vector <
   //   unsigned int, SinkInterface* > SinkComponentsContainerType;

   // typedef std::vector <
   //   unsigned int, SourceInterface* > SourceComponentsContainerType;

    typedef itk::VectorContainer <
      unsigned int, ComponentType::Pointer > ComponentsContainerType;

    void SetBlueprint(const Blueprint::Pointer);
    bool Configure();
    bool Execute();
  protected:
    
    Overlord();
    virtual ~Overlord() {};
  private:
    void ApplyNodeConfiguration();
    void ApplyConnectionConfiguration();
    bool UpdateSelectors();
    bool ConnectComponents();
    bool FindRunRegistration();
    bool ConnectSources();
    bool ConnectSinks();
    bool RunRegistrations();
    Blueprint::Pointer m_Blueprint;
    ComponentSelectorContainerType m_ComponentSelectorContainer;
    //ObjectContainerType::Pointer m_InputObjects;
    //ObjectContainerType::Pointer m_OutputObjects;
    //SinkComponentsContainerType::Pointer m_SinkComponents;
    //SourceComponentsContainerType::Pointer m_SourceComponents;
    Reader3doubleContainerType::Pointer m_Readers3double;
    Writer3doubleContainerType::Pointer m_Writers3double;
    Reader2floatContainerType::Pointer m_Readers2float;
    Writer2floatContainerType::Pointer m_Writers2float;
    ComponentsContainerType::Pointer m_RunRegistrationComponents;
  };

} // end namespace selx


#endif // Overlord_h
