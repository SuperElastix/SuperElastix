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

#include "selxNetworkBuilder.h"
#include "selxKeys.h"
#include "selxSuperElastixComponent.h"
#include "selxLoggerImpl.h"

namespace selx
{
template< typename ComponentList >
NetworkBuilder< ComponentList >::NetworkBuilder( LoggerImpl & logger, BlueprintImpl & blueprint ) : m_Logger( logger ), m_isConfigured( false ), m_Blueprint( blueprint ), m_Name( "NetworkBuilder" )
{
}


template< typename ComponentList >
bool
NetworkBuilder< ComponentList >::AddBlueprint( const BlueprintImpl & blueprint )
{
  this->m_Blueprint.ComposeWith( blueprint );
  //m_Blueprint = std::make_shared< BlueprintImpl >(*blueprint);
  return true;
}


template< typename ComponentList >
bool
NetworkBuilder< ComponentList >::Configure()
{
  // Instantiates all the components as described in the blueprint. Returns true
  // if all components could be uniquely selected.
  // Configuration consists of 3 steps:
  // - ApplyNodeConfiguration()
  // - ApplyConnectionConfiguration()
  // - PropagateConnectionsWithUniqueComponents();

  if( !this->m_isConfigured )
  {
    this->m_Logger.Log( LogLevel::INF, "Applying component criteria ... " );
    this->ApplyComponentConfiguration();
    auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    this->m_Logger.Log(  LogLevel::INF,
                         "Applying component criteria ... Done. {0:d} out of {1:d} components was uniquely selected.",
                         m_Blueprint.GetComponentNames().size()-nonUniqueComponentNames.size(),
                         m_Blueprint.GetComponentNames().size() );

    this->m_Logger.Log( LogLevel::INF, "Applying connection criteria ..." );
    this->ApplyConnectionConfiguration();
    nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    this->m_Logger.Log(  LogLevel::INF,
                         "Applying connection criteria ... Done. {0:d} out of {1:d} components could not be uniquely selected.",
                         nonUniqueComponentNames.size(),
                         m_Blueprint.GetComponentNames().size() );

    this->m_Logger.Log( LogLevel::INF, "Performing handshakes between unique and non-unique Components ..." );
    this->PropagateConnectionsWithUniqueComponents();
    nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    this->m_Logger.Log(  LogLevel::INF,
                         "Performing handshakes between unique and non-unique Components ... Done. {0:d} out of {1:d} components was uniquely selected.",
                         m_Blueprint.GetComponentNames().size()-nonUniqueComponentNames.size(),
                         m_Blueprint.GetComponentNames().size() );
    this->m_isConfigured = true;
  }
  auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();

  if( nonUniqueComponentNames.size() > 0 )
  {
    this->m_Logger.Log( LogLevel::CRT, "These nodes need more criteria: [" );
    for( const auto & nonUniqueComponentName : nonUniqueComponentNames )
    {
      this->m_Logger.Log( LogLevel::CRT, "  %s: %d",
                          nonUniqueComponentName,
                          this->m_ComponentSelectorContainer[ nonUniqueComponentName ]->NumberOfComponents() );
    }
    this->m_Logger.Log( LogLevel::CRT, "]" );
    return false;
  }

  return true;
}


template< typename ComponentList >
NetworkBuilderBase::ComponentNamesType
NetworkBuilder< ComponentList >::GetNonUniqueComponentNames()
{
  ComponentNamesType                  nonUniqueComponentNames;
  const BlueprintImpl::ComponentNamesType componentNames = m_Blueprint.GetComponentNames();
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


template< typename ComponentList >
void
NetworkBuilder< ComponentList >::ApplyComponentConfiguration()
{
  // Creates a ComponentSelector for each node of the graph and apply
  // the criteria/properties at each node to narrow the Component selection.

  // At the NetworkBuilder we store all components selectors in a mapping based
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

  BlueprintImpl::ComponentNamesType componentNames = this->m_Blueprint.GetComponentNames();

  for( auto const & name : componentNames )
  {
    ComponentSelectorPointer currentComponentSelector = std::make_shared< ComponentSelectorType >( name, this->m_Logger );

    BlueprintImpl::ParameterMapType currentProperty = this->m_Blueprint.GetComponent( name );
    for( auto const& criterion : currentProperty )
    {
      currentComponentSelector->AddCriterion( criterion );
      
      this->m_Logger.Log( LogLevel::DBG,
                          "Finding component for \"{0}\": {1} component(s) satisfies {2} equal {3} and previous criteria.",
                          name,
                          currentComponentSelector->NumberOfComponents(),
                          criterion.first,
                          this->m_Logger.m_StreamToString << criterion.second);
    }

    if( currentComponentSelector->NumberOfComponents() == 0 )
    {
      this->m_Logger.Log( LogLevel::CRT, "Could not identify unique component {0} from the specified criteria." );
      throw std::runtime_error( "See the log for details." );
    }

    // insert new element
    this->m_ComponentSelectorContainer[ name ] = currentComponentSelector;
  }
  return;
}


template< typename ComponentList >
void
NetworkBuilder< ComponentList >::ApplyConnectionConfiguration()
{
  // Read the criteria/properties at each Connection and narrow the selection of
  // components.
  // It does not necessarily mean that if connected Components fulfill the connection
  // criteria they can be connected eventually. A criterion at a Connection could
  // be e.g. that Dimensionality equals 3. The providing Component could have 1 (or more)
  // interface that is of that dimensionality and the accepting interface as well, but the
  // interfaces could still be of different types (including different other template arguments)

  BlueprintImpl::ComponentNamesType componentNames = this->m_Blueprint.GetComponentNames();
  for( auto const & acceptingComponent : componentNames )
  {
    for( auto const & providingComponent : this->m_Blueprint.GetOutputNames( acceptingComponent ) )
    {
      BlueprintImpl::ParameterMapType connectionProperties = this->m_Blueprint.GetConnection( acceptingComponent, providingComponent );

      // TODO: Is there a more elegant alternative to manual convertion between types?
      ComponentBase::InterfaceCriteriaType interfaceCriteria;
      for( const auto& connectionProperty : connectionProperties )
      {
        assert( connectionProperty.second.size() <= 1 );
        if( connectionProperty.second.size() == 1 ) {
          interfaceCriteria[connectionProperty.first] = connectionProperty.second[0];
        }
      }

      this->m_ComponentSelectorContainer[ acceptingComponent ]->AddProvidingInterfaceCriteria( interfaceCriteria );
      this->m_ComponentSelectorContainer[ providingComponent ]->AddAcceptingInterfaceCriteria( interfaceCriteria );

      if( !interfaceCriteria.empty() )
      {
        this->m_Logger.Log( LogLevel::DBG,
                            "{0} can connect to {1} via {2}:",
                            providingComponent,
                            acceptingComponent,
                            this->m_Logger.m_StreamToString << interfaceCriteria );
      }

      if( this->m_ComponentSelectorContainer[ providingComponent ]->NumberOfComponents() == 0 )
      {
        std::string msg = providingComponent + "does not provide any connections with the given criteria.";
        this->m_Logger.Log( LogLevel::ERR, msg );
        throw std::runtime_error( msg );
      }

      if( this->m_ComponentSelectorContainer[ acceptingComponent ]->NumberOfComponents() == 0 )
      {
        std::string msg = acceptingComponent + "does not accept any connections with the given criteria.";
        this->m_Logger.Log( LogLevel::ERR, msg );
      }
    }
  }

  return;
}


template< typename ComponentList >
void
NetworkBuilder< ComponentList >::PropagateConnectionsWithUniqueComponents()
{
  // Narrow down the selection of non-uniquely selected components by finding all
  // connections to a component that is uniquely selected and verifying the matching interface types.
  bool anySelectionNarrowed( true );

  while( anySelectionNarrowed )
  {
    anySelectionNarrowed = false;
    auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    for( auto const & name : nonUniqueComponentNames )
    {
      // check all components that accept from component "name"
      for( auto const & outgoingName : this->m_Blueprint.GetOutputNames( name ) )
      {
        // if the accepting component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
        if( std::find( nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), outgoingName ) == nonUniqueComponentNames.end() )
        {
          BlueprintImpl::ParameterMapType          connectionProperties = this->m_Blueprint.GetConnection( name, outgoingName );
          ComponentBase::InterfaceCriteriaType interfaceCriteria;
          //TODO:
          //1: this lambda function converts the blueprint properties: map<string,vector<string>> to interfacecriteria: map<string,string>, consider redesign.
          //2: connection blueprint->addConnection("myfirstnode","mysecondnode",{{}}) creates connectionProperties {"",[]} which is not an empty map.
          std::for_each( connectionProperties.begin(), connectionProperties.end(), [ interfaceCriteria ](
              BlueprintImpl::ParameterMapType::value_type kv ) mutable {
              if( kv.second.size() > 0 )
              {
                interfaceCriteria[ kv.first ] = kv.second[ 0 ];
              }
            } );

          auto               outgoingComponent = this->m_ComponentSelectorContainer[ outgoingName ]->GetComponent();
          const unsigned int beforeCriteria    = this->m_ComponentSelectorContainer[ name ]->NumberOfComponents();
          this->m_ComponentSelectorContainer[ name ]->RequireProvidingInterfaceTo( outgoingComponent, interfaceCriteria );
          const unsigned int afterCriteria = this->m_ComponentSelectorContainer[ name ]->NumberOfComponents();

          std::cout << afterCriteria << " " << name << " components can Provide to " << outgoingName << std::endl;

          if( beforeCriteria > afterCriteria )
          {
            anySelectionNarrowed = true;
          }
        }
      }
      // check all components that provide to component "name"
      for( auto const & incomingName : this->m_Blueprint.GetInputNames( name ) )
      {
        // if the providing component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
        if( std::find( nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), incomingName ) == nonUniqueComponentNames.end() )
        {
          //std::cout << "Check which " << name << " component can Accept from " << incomingName << std::endl;
          BlueprintImpl::ParameterMapType          connectionProperties = this->m_Blueprint.GetConnection( incomingName, name );
          ComponentBase::InterfaceCriteriaType interfaceCriteria;
          //TODO:
          //1: this lambda function converts the blueprint properties: map<string,vector<string>> to interfacecriteria: map<string,string>, consider redesign.
          //2: connection blueprint->addConnection("myfirstnode","mysecondnode",{{}}) creates connectionProperties {"",[]} which is not an empty map.
          std::for_each( connectionProperties.begin(), connectionProperties.end(), [ &interfaceCriteria ](
              BlueprintImpl::ParameterMapType::value_type kv ) mutable {
              if( kv.second.size() > 0 )
              {
                interfaceCriteria[ kv.first ] = kv.second[ 0 ];
              }
            } );

          auto incomingComponent = this->m_ComponentSelectorContainer[ incomingName ]->GetComponent();

          const unsigned int beforeCriteria = this->m_ComponentSelectorContainer[ name ]->NumberOfComponents();
          this->m_ComponentSelectorContainer[ name ]->RequireAcceptingInterfaceFrom( incomingComponent, interfaceCriteria );
          const unsigned int afterCriteria = this->m_ComponentSelectorContainer[ name ]->NumberOfComponents();
          std::cout << afterCriteria << " " << name << " components can Accept from " << incomingName << std::endl;

          if( beforeCriteria > afterCriteria )
          {
            anySelectionNarrowed = true;
          }
        }
      }
    }
  }
}


template< typename ComponentList >
bool
NetworkBuilder< ComponentList >::ConnectComponents()
{
  bool isAllSuccess = true;

  BlueprintImpl::ComponentNamesType componentNames = this->m_Blueprint.GetComponentNames();
  for( auto const & name : componentNames )
  {
    for( auto const & outgoingName : this->m_Blueprint.GetOutputNames( name ) )
    {
      //TODO check direction upstream/downstream input/output source/target
      //TODO GetComponent returns NULL if possible components !=1 we can check for that, but NetworkBuilder::UpdateSelectors() does something similar.
      ComponentBase::Pointer sourceComponent = this->m_ComponentSelectorContainer[ name ]->GetComponent();
      ComponentBase::Pointer targetComponent = this->m_ComponentSelectorContainer[ outgoingName ]->GetComponent();

      BlueprintImpl::ParameterMapType          connectionProperties = this->m_Blueprint.GetConnection( name, outgoingName );
      ComponentBase::InterfaceCriteriaType interfaceCriteria;
      std::for_each( connectionProperties.begin(), connectionProperties.end(), [ &interfaceCriteria ](
          BlueprintImpl::ParameterMapType::value_type kv ) mutable {
          if( kv.second.size() > 0 )
          {
            interfaceCriteria[ kv.first ] = kv.second[ 0 ];
          }
        } );

      int numberOfConnections = targetComponent->AcceptConnectionFrom( sourceComponent, interfaceCriteria );

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


template< typename ComponentList >
bool
NetworkBuilder< ComponentList >::CheckConnectionsSatisfied()
{
  bool isAllSatisfied = true;

  BlueprintImpl::ComponentNamesType componentNames = this->m_Blueprint.GetComponentNames();
  for( auto const & name : componentNames )
  {
    ComponentBase::Pointer component   = this->m_ComponentSelectorContainer[ name ]->GetComponent();
    bool                   isSatisfied = component->ConnectionsSatisfied();
    if( isSatisfied == false )
    {
      isAllSatisfied = false;
      std::cout << "Component " << name << " has unsatisfied connections" << std::endl;
    }
  }
  return isAllSatisfied;
}


template< typename ComponentList >
NetworkBuilderBase::SourceInterfaceMapType
NetworkBuilder< ComponentList >::GetSourceInterfaces()
{
  /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */

  SourceInterfaceMapType sourceInterfaceMap;
  for( const auto & componentSelector : this->m_ComponentSelectorContainer )
  {
    ComponentBase::Pointer component = componentSelector.second->GetComponent();

    if( component->CountProvidingInterfaces( { { keys::NameOfInterface, keys::SourceInterface } } ) == 1 )
    {
      SourceInterface::Pointer provingSourceInterface = std::dynamic_pointer_cast< SourceInterface >( component );
      if( !provingSourceInterface )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        throw std::runtime_error( "dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't" );
      }
      sourceInterfaceMap[ componentSelector.first ] = provingSourceInterface;
    }
  }
  return sourceInterfaceMap;
}


template< typename ComponentList >
NetworkBuilderBase::SinkInterfaceMapType
NetworkBuilder< ComponentList >::GetSinkInterfaces()
{
  /** Scans all Components to find those with Sinking capability and store them in SinkComponents list */

  SinkInterfaceMapType sinkInterfaceMap;
  for( auto const & componentSelector : this->m_ComponentSelectorContainer )
  {
    ComponentBase::Pointer component = componentSelector.second->GetComponent();
    if( component->CountProvidingInterfaces( { { keys::NameOfInterface, keys::SinkInterface } } ) == 1 )
    {
      SinkInterface::Pointer provingSinkInterface = std::dynamic_pointer_cast< SinkInterface >( component );
      if( !provingSinkInterface )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        throw std::runtime_error( "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't" );
      }
      sinkInterfaceMap[ componentSelector.first ] = provingSinkInterface;
    }
  }
  return sinkInterfaceMap;
}


template< typename ComponentList >
AnyFileReader::Pointer
NetworkBuilder< ComponentList >::GetInputFileReader( const NetworkBuilderBase::ComponentNameType & inputName )
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


template< typename ComponentList >
AnyFileWriter::Pointer
NetworkBuilder< ComponentList >::GetOutputFileWriter( const NetworkBuilderBase::ComponentNameType & outputName )
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


template< typename ComponentList >
SinkInterface::DataObjectPointer
NetworkBuilder< ComponentList >::GetInitializedOutput( const NetworkBuilderBase::ComponentNameType & outputName )
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


template< typename ComponentList >
NetworkContainer
NetworkBuilder< ComponentList >::GetRealizedNetwork()
{
  // vector that stores all components
  std::vector< ComponentBase::Pointer > components;

  std::map< std::string, itk::DataObject::Pointer > outputObjectsMap;

  if( this->Configure() )
  {
    for( const auto & componentSelector : this->m_ComponentSelectorContainer )
    {
      //store all components
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      components.push_back( component );

      /** Scans all Components to find those with Sinking capability and store the outputs in outputObjectsMap */
      if( component->CountProvidingInterfaces( { { keys::NameOfInterface, keys::SinkInterface } } ) == 1 )
      {
        SinkInterface::Pointer provingSinkInterface = std::dynamic_pointer_cast< SinkInterface >( component );
        if( !provingSinkInterface )   // is actually a double-check for sanity: based on criterion cast should be successful
        {
          throw std::runtime_error( "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't" );
        }
        outputObjectsMap[ componentSelector.first ] = provingSinkInterface->GetMiniPipelineOutput();
      }
    }

    return NetworkContainer( components, outputObjectsMap );
  }
  else
  {
    std::stringstream msg;
    msg << "Network is not realized yet";
    throw std::runtime_error( msg.str() );
    return NetworkContainer( components, outputObjectsMap );
  }
}
} // end namespace selx
