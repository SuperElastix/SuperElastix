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

#include "Overlord.h"
#include "selxKeys.h"

namespace selx
{
  Overlord::Overlord() : m_isConfigured(false)
  {
  }

  bool Overlord::Configure()
  {
    //TODO: make a while loop until stable:
    // - for each unique component propagate the required interfaces to neighboring components as added criterion   
    
    
    if (!this->m_isConfigured)
    {

      std::cout << "Applying Component Criteria" << std::endl;
      this->ApplyNodeConfiguration();

      auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
      std::cout << nonUniqueComponentNames.size() << " out of " << m_Blueprint->GetComponentNames().size() << " Components could not be uniquely selected" << std::endl << std::endl;

      std::cout << "Applying Connection Criteria" << std::endl;
      this->ApplyConnectionConfiguration();
      nonUniqueComponentNames = this->GetNonUniqueComponentNames();
      std::cout << nonUniqueComponentNames.size() << " out of " << m_Blueprint->GetComponentNames().size() << " Components could not be uniquely selected" << std::endl << std::endl;

      this->m_isConfigured = true;
    }
    auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    if (nonUniqueComponentNames.size() > 0)
    {
      std::cout << std::endl << "These Nodes need more criteria: " << std::endl; 
      for (const auto & nonUniqueComponentName : nonUniqueComponentNames)
      {
        std::cout << nonUniqueComponentName<< std::endl;
      }
      return false;
    }
    return true;
  }

  Overlord::ComponentNamesType Overlord::GetNonUniqueComponentNames()
  {
    ComponentNamesType nonUniqueComponentNames;
    const Blueprint::ComponentNamesType componentNames = m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {    
     
      // The current idea of the configuration setup is that the number of 
      // possible components at a node can only be reduced by adding criteria.
      // If a node has 0 possible components, the configuration is aborted (with an exception)
      // If all nodes have exactly 1 possible component, no more criteria are needed.

      if (this->m_ComponentSelectorContainer[name]->HasMultipleComponents()==true)
      {
        //std::cout << "To select a component for blueprint node " << name << " more critatia are required" << std::endl;
        nonUniqueComponentNames.push_back(name);
      }
    }
    return nonUniqueComponentNames;
  }

  void Overlord::ApplyNodeConfiguration()
  {
    // At the overlord we store all components selectors in a mapping based  
    // on the keys we find in the graph. This is a flexible solution, but is
    // fragile as well since correspondence is implicit.
    // We might consider copying the blueprint graph to a component selector
    // graph, such that all graph operations correspond
    //
    // This could be in line with the idea of maintaining 2 graphs: 
    // 1 descriptive (= const blueprint) and 1 internal holding to realized 
    // components.
    // Manipulating the internal graph (combining component nodes into a 
    // hybrid node, duplicating sub graphs, etc) is possible then.
    //
    // Additional redesign consideration: the final graph should hold the 
    // realized components at each node and not the ComponentSelectors that, 
    // in turn, hold 1 (or more) component.

    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    
    for (auto const & name : componentNames)
    {
      std::cout << " Blueprint Node: " << name << std::endl;
      ComponentSelectorPointer currentComponentSelector = ComponentSelector::New();
      Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetComponent(name);
      for (auto const & criterion : currentProperty)
      {
        std::cout << "  " << criterion.first << ": " << criterion.second[0] << std::endl;
        currentComponentSelector->AddCriterion(criterion);
      }

      if ((currentComponentSelector->HasMultipleComponents() == false) && (currentComponentSelector->GetComponent().IsNull()))
      {
        std::stringstream msg;
        msg << "Too many criteria for Component " << name << std::endl;
        throw std::runtime_error::runtime_error(msg.str());
      }

      // insert new element
      this->m_ComponentSelectorContainer[name] = currentComponentSelector;
    }
    return;
  }

  void Overlord::ApplyConnectionConfiguration()
  {
    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      for (auto const & outgoingName : this->m_Blueprint->GetOutputNames(name))
      {
        //TODO check direction upstream/downstream input/output source/target
        Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);
        if (connectionProperties.count("NameOfInterface") > 0)
        {
          
          this->m_ComponentSelectorContainer[name]->AddCriterion({ keys::HasProvidingInterface, connectionProperties[keys::NameOfInterface] });
          this->m_ComponentSelectorContainer[outgoingName]->AddCriterion({ keys::HasAcceptingInterface, connectionProperties[keys::NameOfInterface] });
          std::cout << " Blueprint Node: " << name << std::endl << "  HasProvidingInterface " << connectionProperties[keys::NameOfInterface][0] << std::endl;
          std::cout << " Blueprint Node: " << outgoingName << std::endl << "  HasAcceptingInterface " << connectionProperties[keys::NameOfInterface][0] << std::endl;
        }
      }
      if ((this->m_ComponentSelectorContainer[name]->HasMultipleComponents() == false) && (this->m_ComponentSelectorContainer[name]->GetComponent().IsNull()))
      {
        std::stringstream msg;
        msg << "Too many criteria for Component " << name << std::endl;
        throw std::runtime_error::runtime_error(msg.str());
      }
    }

    return;
  }
  bool Overlord::ConnectComponents()
  {
    bool isAllSuccess = true;

    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      for (auto const & outgoingName : this->m_Blueprint->GetOutputNames(name))
      {
        //TODO check direction upstream/downstream input/output source/target
        //TODO GetComponent returns NULL if possible components !=1 we can check for that, but Overlord::UpdateSelectors() does something similar.
        ComponentBase::Pointer sourceComponent = this->m_ComponentSelectorContainer[name]->GetComponent();
        ComponentBase::Pointer targetComponent = this->m_ComponentSelectorContainer[outgoingName]->GetComponent();

        Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);
        int numberOfConnections = 0;
        if (connectionProperties.count(keys::NameOfInterface) > 0)
        {
          // connect only via interfaces provided by user configuration
          for (auto const & interfaceName : connectionProperties[keys::NameOfInterface])
          {
            numberOfConnections += (targetComponent->AcceptConnectionFrom(interfaceName.c_str(), sourceComponent) == ComponentBase::interfaceStatus::success ? 1: 0);
          }
        }
        else
        {
          // connect via all possible interfaces
          numberOfConnections = targetComponent->AcceptConnectionFrom(sourceComponent);
        }

        if (numberOfConnections == 0)
        {
          isAllSuccess = false;
          std::cout << "Warning: a connection from " << name << " to " << outgoingName << " was specified, but no compatible interfaces were found." << std::endl;
        }
      }
    }
    return isAllSuccess;
  }
  Overlord::SourceInterfaceMapType Overlord::GetSourceInterfaces()
  {
    /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */
    
    SourceInterfaceMapType sourceInterfaceMap;
    for (const auto & componentSelector : this->m_ComponentSelectorContainer)
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriterionBase({ keys::HasProvidingInterface, { keys::SourceInterface } }))
      {
        SourceInterface* provingSourceInterface = dynamic_cast<SourceInterface*> (component.GetPointer());
        if (provingSourceInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          throw std::runtime_error::runtime_error("dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't");
        }
        sourceInterfaceMap[componentSelector.first]=provingSourceInterface;
        
      }
    }
    return sourceInterfaceMap;
  }

  Overlord::SinkInterfaceMapType Overlord::GetSinkInterfaces()
  {
    /** Scans all Components to find those with Sinking capability and store them in SinkComponents list */
   
    SinkInterfaceMapType sinkInterfaceMap;
    for (auto const & componentSelector : this->m_ComponentSelectorContainer)
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriterionBase({ keys::HasProvidingInterface, { keys::SinkInterface } }))
      {
        SinkInterface* provingSinkInterface = dynamic_cast<SinkInterface*> (component.GetPointer());
        if (provingSinkInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          throw std::runtime_error::runtime_error("dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't");
        }
        sinkInterfaceMap[componentSelector.first] = provingSinkInterface;
      }
    }
    return sinkInterfaceMap;
  }
  
  void Overlord::Execute()
  {
       
    /** Scans all Components to find those with RegistrationControllerStart capability and call them */
    const CriterionType criterion = CriterionType(keys::HasProvidingInterface, { keys::RegistrationControllerStartInterface });

    for (auto const & componentSelector : this->m_ComponentSelectorContainer)
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriterionBase(criterion))
      {
        RegistrationControllerStartInterface* providingInterface = dynamic_cast<RegistrationControllerStartInterface*> (component.GetPointer());
        if (providingInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          throw std::runtime_error::runtime_error("dynamic_cast<RegistrationControllerStartInterface*> fails, but based on component criterion it shouldn't");
        }
        providingInterface->RegistrationControllerStart();
      }
    }

  }

  AnyFileReader::Pointer Overlord::GetInputFileReader(const Overlord::ComponentNameType& inputName)
  {
    SourceInterfaceMapType sources = this->GetSourceInterfaces();
    if (sources.count(inputName) != 1)
    {
      std::stringstream msg;
      msg << "No Source component found by name:" << inputName;
      throw std::runtime_error::runtime_error(msg.str());
    }

    return sources[inputName]->GetInputFileReader();
  }
  
  AnyFileWriter::Pointer Overlord::GetOutputFileWriter(const Overlord::ComponentNameType& outputName)
  {
    SinkInterfaceMapType sinks = this->GetSinkInterfaces();
    if (sinks.count(outputName) != 1)
    {
      std::stringstream msg;
      msg << "No Sink component found by name : " << outputName;
      throw std::runtime_error::runtime_error(msg.str());
    }

    return sinks[outputName]->GetOutputFileWriter();
  }

  SinkInterface::DataObjectPointer Overlord::GetInitializedOutput(const Overlord::ComponentNameType& outputName)
  {
    SinkInterfaceMapType sinks = this->GetSinkInterfaces();
    if (sinks.count(outputName) != 1)
    {
      std::stringstream msg;
      msg << "No Sink component found by name : " << outputName;
      throw std::runtime_error::runtime_error(msg.str());
    }

    return sinks[outputName]->GetInitializedOutput();
  }

} // end namespace selx

