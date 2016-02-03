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
    bool FindSources();
    bool FindSinks();
    bool FindRunRegistration();
    bool ConnectSources();
    bool ConnectSinks();

    Blueprint::Pointer m_Blueprint;
    ComponentSelectorContainerType m_ComponentSelectorContainer;
    //ObjectContainerType::Pointer m_InputObjects;
    //ObjectContainerType::Pointer m_OutputObjects;
    SinkComponentsContainerType::Pointer m_SinkComponents;
    SourceComponentsContainerType::Pointer m_SourceComponents;
    ComponentsContainerType::Pointer m_RunRegistrationComponents;

    // For testing purposes, all Sources are connected to an ImageReader
    itk::ImageFileReader<itk::Image<double, 3>>::Pointer m_reader; 

    // For testing purposes, all Sources are connected to an ImageWriter
    itk::ImageFileWriter<itk::Image<double, 3>>::Pointer m_writer;

  };

} // end namespace selx


#endif // Overlord_h
