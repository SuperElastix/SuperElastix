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

#include "selxOverlord.h"
#include "selxKeys.h"

//todo remove
#include "selxInterfaces.h"
#include "selxSuperElastixComponent.h"

namespace selx
{
  
Overlord::Overlord() : m_isConfigured( false )
{
}


bool
Overlord::Configure()
{
  //TODO: make a while loop until stable:
  // - for each unique component propagate the required interfaces to neighboring components as added criterion

  if( !this->m_isConfigured )
  {
    std::cout << "===== Applying Component Criteria =====" << std::endl;
    this->ApplyNodeConfiguration();

    auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    std::cout << nonUniqueComponentNames.size() << " out of " << m_Blueprint->GetComponentNames().size()
              << " Components could not be uniquely selected" << std::endl << std::endl;

    std::cout << "===== Applying Connection Criteria =====" << std::endl;
    this->ApplyConnectionConfiguration();
    nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    std::cout << nonUniqueComponentNames.size() << " out of " << m_Blueprint->GetComponentNames().size()
              << " Components could not be uniquely selected" << std::endl << std::endl;

    std::cout << "===== Performing Handshakes between unique and non-unique Components =====" << std::endl;
    bool anySelectionNarrowed(true);

    while (anySelectionNarrowed){
      anySelectionNarrowed = false;
      nonUniqueComponentNames = this->GetNonUniqueComponentNames();
      for (auto const & name : nonUniqueComponentNames)
      {
        // check all components that accept from component "name"
        for (auto const & outgoingName : this->m_Blueprint->GetOutputNames(name)) 
        {
          // if the accepting component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
          if (std::find(nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), outgoingName) == nonUniqueComponentNames.end())
          {

            Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);
            ComponentBase::InterfaceCriteriaType interfaceCriteria;
            //TODO:
            //1: this lambda function converts the blueprint properties: map<string,vector<string>> to interfacecriteria: map<string,string>, consider redesign.
            //2: connection blueprint->addConnection("myfirstnode","mysecondnode",{{}}) creates connectionProperties {"",[]} which is not an empty map.
            std::for_each(connectionProperties.begin(), connectionProperties.end(), [interfaceCriteria](Blueprint::ParameterMapType::value_type kv) mutable { if (kv.second.size() > 0) interfaceCriteria[kv.first] = kv.second[0]; });

            auto outgoingComponent = this->m_ComponentSelectorContainer[outgoingName]->GetComponent();
            const unsigned int beforeCriteria = this->m_ComponentSelectorContainer[name]->NumberOfComponents();
            this->m_ComponentSelectorContainer[name]->RequireProvidingInterfaceTo(outgoingComponent, interfaceCriteria);
            const unsigned int afterCriteria = this->m_ComponentSelectorContainer[name]->NumberOfComponents();

            std::cout << afterCriteria << " " << name << " components can Provide to " << outgoingName << std::endl;

            if (beforeCriteria > afterCriteria)
            {
              anySelectionNarrowed = true;
            }
          }
        }
        // check all components that provide to component "name"
        for (auto const & incomingName : this->m_Blueprint->GetInputNames(name))
        {
          // if the providing component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
          if (std::find(nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), incomingName) == nonUniqueComponentNames.end())
          {
            //std::cout << "Check which " << name << " component can Accept from " << incomingName << std::endl;
            Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(incomingName, name);
            ComponentBase::InterfaceCriteriaType interfaceCriteria;
            //TODO:
            //1: this lambda function converts the blueprint properties: map<string,vector<string>> to interfacecriteria: map<string,string>, consider redesign.
            //2: connection blueprint->addConnection("myfirstnode","mysecondnode",{{}}) creates connectionProperties {"",[]} which is not an empty map.
            std::for_each(connectionProperties.begin(), connectionProperties.end(), [&interfaceCriteria](Blueprint::ParameterMapType::value_type kv) mutable { if (kv.second.size() > 0) interfaceCriteria[kv.first] = kv.second[0]; });

            auto incomingComponent = this->m_ComponentSelectorContainer[incomingName]->GetComponent();
            
            const unsigned int beforeCriteria = this->m_ComponentSelectorContainer[name]->NumberOfComponents();
            this->m_ComponentSelectorContainer[name]->RequireAcceptingInterfaceFrom(incomingComponent, interfaceCriteria);
            const unsigned int afterCriteria = this->m_ComponentSelectorContainer[name]->NumberOfComponents();
            std::cout << afterCriteria << " " << name << " components can Accept from " << incomingName << std::endl;

            if (beforeCriteria > afterCriteria)
            {
              anySelectionNarrowed = true;
            }
          }
        }
      }
    }
    this->m_isConfigured = true;
  }
  auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
  
  std::cout << nonUniqueComponentNames.size() << " out of " << m_Blueprint->GetComponentNames().size()
    << " Components could not be uniquely selected" << std::endl << std::endl;

  if( nonUniqueComponentNames.size() > 0 )
  {
    std::cout << std::endl << "These Nodes need more criteria: " << std::endl;
    for( const auto & nonUniqueComponentName : nonUniqueComponentNames )
    {
      std::cout << this->m_ComponentSelectorContainer[nonUniqueComponentName]->NumberOfComponents() << "  " << nonUniqueComponentName << std::endl;
    }
    return false;
  }
  return true;
}


Overlord::ComponentNamesType
Overlord::GetNonUniqueComponentNames()
{
  ComponentNamesType                  nonUniqueComponentNames;
  const Blueprint::ComponentNamesType componentNames = m_Blueprint->GetComponentNames();
  for( auto const & name : componentNames )
  {
    // The current idea of the configuration setup is that the number of
    // possible components at a node can only be reduced by adding criteria.
    // If a node has 0 possible components, the configuration is aborted (with an exception)
    // If all nodes have exactly 1 possible component, no more criteria are needed.

    if( this->m_ComponentSelectorContainer[ name ]->NumberOfComponents() > 1 )
    {
      //std::cout << "To select a component for blueprint node " << name << " more critatia are required" << std::endl;
      nonUniqueComponentNames.push_back( name );
    }
  }
  return nonUniqueComponentNames;
}


void
Overlord::ApplyNodeConfiguration()
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

  for( auto const & name : componentNames )
  {
    std::cout << " Blueprint Node: " << name << std::endl;
    ComponentSelectorPointer    currentComponentSelector = ComponentSelector::New();
    Blueprint::ParameterMapType currentProperty          = this->m_Blueprint->GetComponent( name );
    for( auto const & criterion : currentProperty )
    {
      currentComponentSelector->AddCriterion( criterion );
      
      std::cout << "  " << currentComponentSelector->NumberOfComponents() << "  " << criterion.first << ": ";
      if( criterion.second.size() > 1 )
      {
        std::cout << "[ ";
        for( auto const element : criterion.second )
        {
          std::cout << element << " ";
        }
        std::cout << "]";
      }
      else
      {
        std::cout << criterion.second[ 0 ];
      }
      std::cout << std::endl;
      
    }

    if( currentComponentSelector->NumberOfComponents() == 0 )
    {
      std::stringstream msg;
      msg << "Too many criteria for Component " << name << std::endl;
      throw std::runtime_error( msg.str() );
    }

    // insert new element
    this->m_ComponentSelectorContainer[ name ] = currentComponentSelector;
  }
  return;
}


void
Overlord::ApplyConnectionConfiguration()
{
  Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
  for( auto const & name : componentNames )
  {
    for( auto const & outgoingName : this->m_Blueprint->GetOutputNames( name ) )
    {
      Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);

      //TODO:
      //1: this lambda function converts the blueprint properties: map<string,vector<string>> to interfacecriteria: map<string,string>, consider redesign.
      //2: connection blueprint->addConnection("myfirstnode","mysecondnode",{{}}) creates connectionProperties {"",[]} which is not an empty map.
      ComponentBase::InterfaceCriteriaType interfaceCriteria;
      std::for_each(connectionProperties.begin(), connectionProperties.end(), [&interfaceCriteria](Blueprint::ParameterMapType::value_type kv) mutable { if (kv.second.size() > 0) interfaceCriteria[kv.first] = kv.second[0]; });

      this->m_ComponentSelectorContainer[name]->AddProvidingInterfaceCriteria(interfaceCriteria);
      this->m_ComponentSelectorContainer[outgoingName]->AddAcceptingInterfaceCriteria(interfaceCriteria);

      std::cout << " Has Interface: " << std::endl;
      std::for_each(interfaceCriteria.begin(), interfaceCriteria.end(), [](ComponentBase::InterfaceCriteriaType::value_type kv) mutable { std::cout << "  { " << kv.first << ": " << kv.second << " }\n"; });
      std::cout << "  " << this->m_ComponentSelectorContainer[name]->NumberOfComponents() << ' ' <<  name << ": Providing" << std::endl;
      std::cout << "  " << this->m_ComponentSelectorContainer[outgoingName]->NumberOfComponents() << ' ' << outgoingName << ": Accepting" << std::endl;


      if( this->m_ComponentSelectorContainer[ outgoingName ]->NumberOfComponents() == 0 )
      {
        std::stringstream msg;
        msg << outgoingName << " does not accept a connection of given criteria" << std::endl;
        throw std::runtime_error( msg.str() );
      }
      if ( this->m_ComponentSelectorContainer[ name ]->NumberOfComponents() == 0 )
      {
	std::stringstream msg;
	msg << name << " does not provide a connection of given criteria" << std::endl;
	throw std::runtime_error( msg.str() );
      }

    }
  }

  return;
}


bool
Overlord::ConnectComponents()
{
  bool isAllSuccess = true;

  Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
  for( auto const & name : componentNames )
  {
    for( auto const & outgoingName : this->m_Blueprint->GetOutputNames( name ) )
    {
      //TODO check direction upstream/downstream input/output source/target
      //TODO GetComponent returns NULL if possible components !=1 we can check for that, but Overlord::UpdateSelectors() does something similar.
      ComponentBase::Pointer sourceComponent = this->m_ComponentSelectorContainer[ name ]->GetComponent();
      ComponentBase::Pointer targetComponent = this->m_ComponentSelectorContainer[ outgoingName ]->GetComponent();

      Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection( name, outgoingName );
      ComponentBase::InterfaceCriteriaType interfaceCriteria;
      std::for_each(connectionProperties.begin(), connectionProperties.end(), [&interfaceCriteria](Blueprint::ParameterMapType::value_type kv) mutable { if (kv.second.size() > 0) interfaceCriteria[kv.first] = kv.second[0]; });


      int numberOfConnections = targetComponent->AcceptConnectionFrom(sourceComponent, interfaceCriteria);
      
      //todo cleanup: if interfaceCriteria is empty we could call      
      // numberOfConnections = targetComponent->AcceptConnectionFrom( sourceComponent );
      // to connect via all possible interfaces, but the effect is equal to calling AcceptConnectionFrom 
      // with empty interfaceCriteria except for speed and clarity ?


      if( numberOfConnections == 0 )
      {
        isAllSuccess = false;
        std::cout << "Warning: a connection from " << name << " to " << outgoingName << " was specified, but no compatible interfaces were found."
                  << std::endl;
      }
    }
  }
  return isAllSuccess;
}


Overlord::SourceInterfaceMapType
Overlord::GetSourceInterfaces()
{
  /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */

  SourceInterfaceMapType sourceInterfaceMap;
  for( const auto & componentSelector : this->m_ComponentSelectorContainer )
  {
    ComponentBase::Pointer component = componentSelector.second->GetComponent();
    
    if (component->CountProvidingInterfaces({ { keys::NameOfInterface, keys::SourceInterface } }) == 1)
    {
      SourceInterface * provingSourceInterface = dynamic_cast< SourceInterface * >( component.GetPointer() );
      if( provingSourceInterface == nullptr )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        throw std::runtime_error( "dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't" );
      }
      sourceInterfaceMap[ componentSelector.first ] = provingSourceInterface;
    }
  }
  return sourceInterfaceMap;
}


Overlord::SinkInterfaceMapType
Overlord::GetSinkInterfaces()
{
  /** Scans all Components to find those with Sinking capability and store them in SinkComponents list */

  SinkInterfaceMapType sinkInterfaceMap;
  for( auto const & componentSelector : this->m_ComponentSelectorContainer )
  {
    ComponentBase::Pointer component = componentSelector.second->GetComponent();
    if (component->CountProvidingInterfaces({ { keys::NameOfInterface, keys::SinkInterface } }) == 1)
    {
      SinkInterface * provingSinkInterface = dynamic_cast< SinkInterface * >( component.GetPointer() );
      if( provingSinkInterface == nullptr )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        throw std::runtime_error( "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't" );
      }
      sinkInterfaceMap[ componentSelector.first ] = provingSinkInterface;
    }
  }
  return sinkInterfaceMap;
}


void
Overlord::Execute()
{
  /** Scans all Components to find those with RegistrationControllerStart capability and call them */
  for( auto const & componentSelector : this->m_ComponentSelectorContainer )
  {
    ComponentBase::Pointer component = componentSelector.second->GetComponent();
    if (component->CountProvidingInterfaces({ { keys::NameOfInterface, keys::RegistrationControllerStartInterface } }) == 1)
    {
      RegistrationControllerStartInterface * providingInterface = dynamic_cast< RegistrationControllerStartInterface * >( component.GetPointer() );
      if( providingInterface == nullptr )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        throw std::runtime_error( "dynamic_cast<RegistrationControllerStartInterface*> fails, but based on component criterion it shouldn't" );
      }
      providingInterface->RegistrationControllerStart();
    }
  }
}


AnyFileReader::Pointer
Overlord::GetInputFileReader( const Overlord::ComponentNameType & inputName )
{
  SourceInterfaceMapType sources = this->GetSourceInterfaces();
  if( sources.count( inputName ) != 1 )
  {
    std::stringstream msg;
    msg << "No Source component found by name:" << inputName;
    throw std::runtime_error( msg.str() );
  }

  return sources[ inputName ]->GetInputFileReader();
}


AnyFileWriter::Pointer
Overlord::GetOutputFileWriter( const Overlord::ComponentNameType & outputName )
{
  SinkInterfaceMapType sinks = this->GetSinkInterfaces();
  if( sinks.count( outputName ) != 1 )
  {
    std::stringstream msg;
    msg << "No Sink component found by name : " << outputName;
    throw std::runtime_error( msg.str() );
  }

  return sinks[ outputName ]->GetOutputFileWriter();
}


SinkInterface::DataObjectPointer
Overlord::GetInitializedOutput( const Overlord::ComponentNameType & outputName )
{
  SinkInterfaceMapType sinks = this->GetSinkInterfaces();
  if( sinks.count( outputName ) != 1 )
  {
    std::stringstream msg;
    msg << "No Sink component found by name : " << outputName;
    throw std::runtime_error( msg.str() );
  }

  return sinks[ outputName ]->GetInitializedOutput();
}
} // end namespace selx
