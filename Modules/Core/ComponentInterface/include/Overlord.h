/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical 
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

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

#include "selxMacro.h"
#include "selxBlueprint.h"
#include "ComponentFactory.h"
#include "Interfaces.h"
namespace selx
{

  class Overlord : public itk::LightProcessObject
  {
  public:

    selxNewMacro(Overlord, itk::LightProcessObject);

    typedef ComponentBase::CriteriaType CriteriaType;
    typedef ComponentBase::CriterionType CriterionType;
    typedef ComponentBase::ParameterValueType ParameterValueType;
    //typedef std::map<std::string, std::string> CriteriaType;
    //typedef std::pair<std::string, std::string> CriterionType;

    typedef ComponentBase       ComponentType;
    typedef ComponentSelector::Pointer ComponentSelectorPointer;
    typedef std::map< Blueprint::ComponentNameType, ComponentSelectorPointer> ComponentSelectorContainerType;
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


    typedef itk::ImageFileWriter<itk::Image<itk::Vector<float, 2>, 2>> WriterDisplacement2floatType;
    typedef itk::ImageFileWriter<itk::Image<itk::Vector<double, 3>, 3>> WriterDisplacement3doubleType;

    typedef itk::VectorContainer <
      unsigned int, WriterDisplacement2floatType::Pointer > WriterDisplacement2floatContainerType;

    typedef itk::VectorContainer <
      unsigned int, WriterDisplacement3doubleType::Pointer > WriterDisplacement3doubleContainerType;


   // typedef itk::Object::Pointer ObjectPointer;
   // typedef itk::VectorContainer <
   //   unsigned int, ObjectPointer > ObjectContainerType;

    //typedef itk::VectorContainer <
    //  unsigned int, SinkInterface* > SinkComponentsContainerType;
    
    //typedef itk::VectorContainer <
    //  unsigned int, SourceInterface* > SourceInterfaceContainerType;

    typedef std::map <
      std::string, SourceInterface2* > SourceInterfaceMapType;

    typedef std::map <
      std::string, SinkInterface2* > SinkInterfaceMapType;

    //TODO these containers are not interface specific
    //typedef itk::VectorContainer <
    //  unsigned int, ComponentType::Pointer > SinkComponentsContainerType;

    //typedef itk::VectorContainer <
    //  unsigned int, ComponentType::Pointer > SourceComponentsContainerType;

   // typedef std::vector <
   //   unsigned int, SinkInterface* > SinkComponentsContainerType;

   typedef std::vector <
      unsigned int, SourceInterface* > SourceComponentsContainerType;

   SourceInterfaceMapType GetSourceInterfaces();
   SinkInterfaceMapType GetSinkInterfaces();

    typedef itk::VectorContainer <
      unsigned int, ComponentType::Pointer > ComponentsContainerType;

    //TODO make const correct
    //itkSetConstObjectMacro(Blueprint, Blueprint);
    itkSetObjectMacro(Blueprint, Blueprint);
    bool Configure();
    bool Execute();

    std::vector<std::string> inputFileNames;
    std::vector<std::string> outputFileNames;
  protected:
    
    Overlord();
    virtual ~Overlord() {};
  public: // temporarily from private to public during refactoring SuperElastixFilter.
    void ApplyNodeConfiguration();
    void ApplyConnectionConfiguration();
    bool UpdateSelectors();
    bool ConnectComponents();
  private:
    bool FindRunRegistration();
    bool FindAfterRegistration();
    bool ConnectSources();
    bool ConnectSinks();
    bool RunRegistrations();
    bool AfterRegistrations();
    //TODO make const correct
    //Blueprint::ConstPointer m_Blueprint;
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
    WriterDisplacement2floatContainerType::Pointer m_WritersDisplacement2float;
    WriterDisplacement3doubleContainerType::Pointer m_WritersDisplacement3double;
    
    ComponentsContainerType::Pointer m_RunRegistrationComponents;
    ComponentsContainerType::Pointer m_AfterRegistrationComponents;
    


  };

} // end namespace selx


#endif // Overlord_h

