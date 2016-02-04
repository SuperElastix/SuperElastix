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

    typedef itk::ImageFileReader<itk::Image<double, 3>> ReaderType;
    typedef itk::ImageFileWriter<itk::Image<double, 3>> WriterType;

    typedef itk::VectorContainer <
      unsigned int, ReaderType::Pointer > ReaderContainerType;

    typedef itk::VectorContainer <
      unsigned int, WriterType::Pointer > WriterContainerType;


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
    ReaderContainerType::Pointer m_Readers;
    WriterContainerType::Pointer m_Writers;
    ComponentsContainerType::Pointer m_RunRegistrationComponents;
  };

} // end namespace selx


#endif // Overlord_h
