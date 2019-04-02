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
NetworkBuilder< ComponentList >::NetworkBuilder( LoggerImpl & logger, const BlueprintImpl & blueprint ) : m_Logger( logger ), m_isConfigured( false ), m_Blueprint( blueprint )
{
}


template< typename ComponentList >
bool
NetworkBuilder< ComponentList >::AddBlueprint( const BlueprintImpl & blueprint )
{
  //Disabled
  //this->m_Blueprint.ComposeWith( blueprint );
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
                         "Applying component criteria ... Done. {0:d} out of {1:d} components were uniquely selected.",
                         m_Blueprint.GetComponentNames().size()-nonUniqueComponentNames.size(),
                         m_Blueprint.GetComponentNames().size() );

    this->m_Logger.Log( LogLevel::INF, "Applying connection criteria ..." );
    this->ApplyConnectionConfiguration();
    nonUniqueComponentNames = this->GetNonUniqueComponentNames();
    this->m_Logger.Log(  LogLevel::INF,
                         "Applying connection criteria ... Done. {0:d} out of {1:d} components were uniquely selected.",
                         m_Blueprint.GetComponentNames().size()-nonUniqueComponentNames.size(),
                         m_Blueprint.GetComponentNames().size() );

    if( nonUniqueComponentNames.size() > 0 )
    {
      this->m_Logger.Log( LogLevel::INF, "Performing handshakes between unique and non-unique component(s) ..." );
      this->PropagateConnectionsWithUniqueComponents();
      nonUniqueComponentNames = this->GetNonUniqueComponentNames();
      this->m_Logger.Log(  LogLevel::INF,
                           "Performing handshakes between unique and non-unique component(s) ... Done. {0:d} out of {1:d} components were uniquely selected.",
                           m_Blueprint.GetComponentNames().size()-nonUniqueComponentNames.size(),
                           m_Blueprint.GetComponentNames().size() );
    }
    this->m_isConfigured = true;
  }

  auto nonUniqueComponentNames = this->GetNonUniqueComponentNames();

  // TODO: Print the available criteria
  if( nonUniqueComponentNames.size() > 0 )
  {
    this->m_Logger.Log( LogLevel::CRT, this->m_Logger.ToString(nonUniqueComponentNames) + " need more criteria." );
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

  for( auto const & componentName : this->m_Blueprint.GetComponentNames() )
  {
    ComponentSelectorPointer currentComponentSelector = std::make_shared< ComponentSelectorType >( componentName, this->m_Logger );

    BlueprintImpl::ParameterMapType currentProperty = this->m_Blueprint.GetComponent( componentName );
    for( auto const& criterion : currentProperty )
    {
      currentComponentSelector->AddCriterion( criterion );
      
      this->m_Logger.Log( LogLevel::DBG,
                          "Finding component for {0}: {1} component(s) satisfies {2} : {3}  and previous criteria.",
                          componentName,
                          currentComponentSelector->NumberOfComponents(),
                          criterion.first,
                          this->m_Logger.ToString(criterion.second));
    }

    if( currentComponentSelector->NumberOfComponents() == 0 )
    {
      std::string msg = "No components fulfill all criteria for " + componentName + ".";
      this->m_Logger.Log( LogLevel::CRT, msg );
      throw std::runtime_error( msg );
    }

    // insert new element
    this->m_ComponentSelectorContainer[ componentName ] = currentComponentSelector;
  }
  return;
}


template<
  typename ComponentList >
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
  for( auto const & providingComponentName : this->m_Blueprint.GetComponentNames() )
  {
    for( auto const & acceptingComponentName : this->m_Blueprint.GetOutputNames( providingComponentName ) )
    {
      for ( auto const & connectionName : this->m_Blueprint.GetConnectionNames( providingComponentName, acceptingComponentName ) )
      {
        BlueprintImpl::ParameterMapType connectionProperties = this->m_Blueprint.GetConnection( providingComponentName, acceptingComponentName, connectionName );

        // TODO: #110
        ComponentBase::InterfaceCriteriaType interfaceCriteria;
        for( const auto& connectionProperty : connectionProperties )
        {
          assert( connectionProperty.second.size() <= 1 );
          if( connectionProperty.second.size() == 1 ) {
            interfaceCriteria[connectionProperty.first] = connectionProperty.second[0];
          }
        }

        // TODO: connectionName in log message
        this->m_ComponentSelectorContainer[ providingComponentName ]->AddProvidingInterfaceCriteria( interfaceCriteria );
        this->m_Logger.Log(LogLevel::DBG,
          "Finding component for {0}: {1} component(s) satisfies ProvidingInterface {2} and previous criteria.",
          providingComponentName,
          this->m_ComponentSelectorContainer[providingComponentName]->NumberOfComponents(),
          this->m_Logger.ToString(interfaceCriteria) );

        this->m_ComponentSelectorContainer[ acceptingComponentName ]->AddAcceptingInterfaceCriteria( interfaceCriteria );
        this->m_Logger.Log(LogLevel::DBG,
          "Finding component for {0}: {1} component(s) satisfies AcceptingInterface {2} and previous criteria.",
          acceptingComponentName,
          this->m_ComponentSelectorContainer[acceptingComponentName]->NumberOfComponents(),
          this->m_Logger.ToString(interfaceCriteria) );

        if( this->m_ComponentSelectorContainer[ acceptingComponentName ]->NumberOfComponents() == 0 )
        {
          std::string msg = acceptingComponentName + "does not provide any connections with the given criteria.";
          this->m_Logger.Log( LogLevel::ERR, msg );
          throw std::runtime_error( msg );
        }

        if( this->m_ComponentSelectorContainer[ providingComponentName ]->NumberOfComponents() == 0 )
        {
          std::string msg = providingComponentName + "does not accept any connections with the given criteria.";
          this->m_Logger.Log( LogLevel::ERR, msg );
          throw std::runtime_error( msg );
        }
      }
    }
  }
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
    for( auto const & componentName : nonUniqueComponentNames )
    {
      // check all components that accept from component "componentName"
      for( auto const & acceptingComponentName : this->m_Blueprint.GetOutputNames( componentName ) )
      {
        // if the accepting component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
        if( std::find( nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), acceptingComponentName ) == nonUniqueComponentNames.end() )
        {
          for ( const auto & connectionName : this->m_Blueprint.GetConnectionNames( componentName, acceptingComponentName ) )
          {
            BlueprintImpl::ParameterMapType connectionProperties = this->m_Blueprint.GetConnection( componentName, acceptingComponentName, connectionName );

            // TODO: #110
            ComponentBase::InterfaceCriteriaType interfaceCriteria;
            for( const auto& connectionProperty : connectionProperties )
            {
              assert( connectionProperty.second.size() <= 1 );
              if( connectionProperty.second.size() == 1 ) {
                interfaceCriteria[connectionProperty.first] = connectionProperty.second[0];
              }
            }

            // TODO: connectionName in log message
            auto               acceptingComponent = this->m_ComponentSelectorContainer[ acceptingComponentName ]->GetComponent();
            const unsigned int beforeCriteria    = this->m_ComponentSelectorContainer[ componentName ]->NumberOfComponents();
            this->m_Logger.Log( LogLevel::DBG, "Propagating 'ProvidingInterface' properties from '{0}' to {2} components at '{1}' ... ", componentName, acceptingComponentName, beforeCriteria );
            this->m_ComponentSelectorContainer[ componentName ]->RequireProvidingInterfaceTo( acceptingComponent, interfaceCriteria );
            const unsigned int afterCriteria = this->m_ComponentSelectorContainer[ componentName ]->NumberOfComponents();
            this->m_Logger.Log( LogLevel::DBG, "Propagating 'ProvidingInterface' properties from '{0}' to {2} components at '{1}' ... Done. Reduced '{1}' to {3} components", componentName, acceptingComponentName, beforeCriteria, afterCriteria );

            if( beforeCriteria > afterCriteria )
            {
              anySelectionNarrowed = true;
            }
            this->m_Logger.Log( LogLevel::TRC, "Selection Narrowed: {} ", anySelectionNarrowed );
            
            if (afterCriteria == 0)
            {
              std::string msg = "No component exists for '" + componentName + "' that has a suitable interface to provide to '" + acceptingComponentName + "'" ;
              this->m_Logger.Log(LogLevel::ERR, msg);
              throw std::runtime_error( msg );
            }
          }
        }
      }
      // check all components that provide to component "componentName"
      for( auto const & providingComponentName : this->m_Blueprint.GetInputNames( componentName ) )
      {
        // if the providing component is also not uniquely selected, we do not try to check all valid combinations, since this would make the handshake logic too complicated
        if( std::find( nonUniqueComponentNames.begin(), nonUniqueComponentNames.end(), providingComponentName ) == nonUniqueComponentNames.end() )
        {
          for (const auto & connectionName : this->m_Blueprint.GetConnectionNames( providingComponentName, componentName ) )
          {
            BlueprintImpl::ParameterMapType connectionProperties = this->m_Blueprint.GetConnection( providingComponentName, componentName, connectionName);

            // TODO:#110
            ComponentBase::InterfaceCriteriaType interfaceCriteria;
            for( const auto& connectionProperty : connectionProperties )
            {
              assert( connectionProperty.second.size() <= 1 );
              if( connectionProperty.second.size() == 1 ) {
                interfaceCriteria[connectionProperty.first] = connectionProperty.second[0];
              }
            }

            // TODO(FB): Use this to provide more info to user
            auto providingComponent = this->m_ComponentSelectorContainer[ providingComponentName ]->GetComponent();

            const unsigned int beforeCriteria = this->m_ComponentSelectorContainer[ componentName ]->NumberOfComponents();
            this->m_Logger.Log(LogLevel::DBG, "Propagating 'AcceptingInterface' properties from '{0}' to {2} components at '{1}' ... ", componentName, providingComponentName, beforeCriteria);
            this->m_ComponentSelectorContainer[ componentName ]->RequireAcceptingInterfaceFrom( providingComponent, interfaceCriteria );
            const unsigned int afterCriteria = this->m_ComponentSelectorContainer[ componentName ]->NumberOfComponents();
            this->m_Logger.Log(LogLevel::DBG, "Propagating 'AcceptingInterface' properties from '{0}' to {2} components at '{1}' ... Done. Reduced '{1}' to {3} components", componentName, providingComponentName, beforeCriteria, afterCriteria);

            if( beforeCriteria > afterCriteria )
            {
              anySelectionNarrowed = true;
            }
            this->m_Logger.Log( LogLevel::TRC, "Selection Narrowed: {} ", anySelectionNarrowed );
            if (afterCriteria == 0)
            {
              std::string msg = "No component exists for '" + componentName + "' that has a suitable interface to accept from '" + providingComponentName + "'";
              this->m_Logger.Log(LogLevel::ERR, msg);
              throw std::runtime_error(msg);
            }

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

  for( auto const & providingComponentName : this->m_Blueprint.GetComponentNames() )
  {
    for( auto const & acceptingComponentName : this->m_Blueprint.GetOutputNames( providingComponentName ) )
    {
      // GetComponent returns NULL if possible components !=1. We assume ComponentSelectorContainers have unique components since Configure().
      ComponentBase::Pointer providingComponent = this->m_ComponentSelectorContainer[ providingComponentName ]->GetComponent();
      ComponentBase::Pointer acceptingComponent = this->m_ComponentSelectorContainer[ acceptingComponentName ]->GetComponent();

      // multiple parallel 'named' connections between 2 components can exist
      auto connectionNames = this->m_Blueprint.GetConnectionNames( providingComponentName, acceptingComponentName );
      for ( auto connectionName : connectionNames )
      {
        BlueprintImpl::ParameterMapType          connectionProperties = this->m_Blueprint.GetConnection( providingComponentName, acceptingComponentName, connectionName );
      
        // TODO:#110
        ComponentBase::InterfaceCriteriaType interfaceCriteria;
        std::for_each( connectionProperties.begin(), connectionProperties.end(), [ &interfaceCriteria ](
            BlueprintImpl::ParameterMapType::value_type kv ) mutable {
            if( kv.second.size() > 0 )
            {
              interfaceCriteria[ kv.first ] = kv.second[ 0 ];
            }
          } );

        std::string message1, message2;
        if (connectionNames.size() > 1) // specialize log messages if multiple parallel connections exist
        {
          message1 = "Connect '{0}' to '{1}' by connection '{2}' ... ";
          message2 = "Connect '{0}' to '{1}' by connection '{3}' ... Done, by {2} interface(s).";
        }
        else
        {
          message1 = "Connect '{0}' to '{1}' ... ";
          message2 = "Connect '{0}' to '{1}' ... Done, by {2} interface(s).";

        }
        this->m_Logger.Log(LogLevel::DBG, message1 , providingComponentName, acceptingComponentName, connectionName);
        int numberOfConnections = acceptingComponent->AcceptConnectionFrom(providingComponent, interfaceCriteria);
        this->m_Logger.Log(LogLevel::DBG, message2 , providingComponentName, acceptingComponentName, numberOfConnections, connectionName);
        if( numberOfConnections == 0 )
        {
          isAllSuccess = false;
          this->m_Logger.Log( LogLevel::CRT, "Connection from '{0}' to '{1}' was specified but no compatible interfaces were found.", providingComponentName, acceptingComponentName);
          throw std::runtime_error("See critical log message");
        }
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

  // TODO: Print the unsatisfied connections
  for( auto const & name : this->m_Blueprint.GetComponentNames() )
  {
    ComponentBase::Pointer component   = this->m_ComponentSelectorContainer[ name ]->GetComponent();
    bool                   isSatisfied = component->ConnectionsSatisfied();
    if( isSatisfied == false )
    {
      isAllSatisfied = false;
      this->m_Logger.Log( LogLevel::ERR, "Component '{0}' has unsatisfied connections.", name );
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

    if(component == nullptr) {
      std::string msg = "No component found for '" + componentSelector.first + "'.";
      this->m_Logger.Log( LogLevel::CRT, msg );
      throw std::runtime_error(msg);
    }

    if( component->CountProvidingInterfaces( { { keys::NameOfInterface, keys::SourceInterface } } ) == 1 )
    {
      SourceInterface::Pointer provingSourceInterface = std::dynamic_pointer_cast< SourceInterface >( component );
      if( !provingSourceInterface )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        this->m_Logger.Log( LogLevel::CRT, "dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't" );
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

    if(component == nullptr) {
      std::string msg = "No component found for '" + componentSelector.first + "'.";
      this->m_Logger.Log( LogLevel::CRT, msg );
      throw std::runtime_error(msg);
    }

    if( component->CountProvidingInterfaces( { { keys::NameOfInterface, keys::SinkInterface } } ) == 1 )
    {
      SinkInterface::Pointer provingSinkInterface = std::dynamic_pointer_cast< SinkInterface >( component );
      if( !provingSinkInterface )  // is actually a double-check for sanity: based on criterion cast should be successful
      {
        this->m_Logger.Log(LogLevel::CRT, "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't");
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
    this->m_Logger.Log(LogLevel::ERR, "{}", msg.str() );
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
    this->m_Logger.Log(LogLevel::ERR, "{}", msg.str() );
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
    this->m_Logger.Log(LogLevel::ERR, "{}", msg.str() );
    throw std::runtime_error( msg.str() );
  }

  return sinks[ outputName ]->GetInitializedOutput();
}


template< typename ComponentList >
NetworkContainer
NetworkBuilder< ComponentList >::GetRealizedNetwork()
{
  // vector that stores all components
  NetworkContainer::ComponentContainerType components;
  NetworkContainer::UpdateOrderType beforeUpdateOrder;
  NetworkContainer::UpdateOrderType updateOrder;
  NetworkContainer::OutputObjectsMapType outputObjectsMap;

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
        auto provingSinkInterface = std::dynamic_pointer_cast< SinkInterface >( component );
        if( !provingSinkInterface )   // is actually a double-check for sanity: based on criterion cast should be successful
        {
          this->m_Logger.Log(LogLevel::CRT, "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't");
          throw std::runtime_error( "dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't" );
        }
        outputObjectsMap[ componentSelector.first ] = provingSinkInterface->GetMiniPipelineOutput();
      }
    }

    for (const auto & componentName : this->m_Blueprint.GetUpdateOrder())
    {
      auto component = this->m_ComponentSelectorContainer[componentName]->GetComponent();
      if (component->CountProvidingInterfaces({ { keys::NameOfInterface, keys::UpdateInterface } }) == 1)
      {
        auto provingUpdateInterface = std::dynamic_pointer_cast<UpdateInterface>(component);
        if (!provingUpdateInterface)   // is actually a double-check for sanity: based on criterion cast should be successful
        {
          this->m_Logger.Log(LogLevel::CRT, "dynamic_cast<provingUpdateInterface*> fails, but based on component criterion it shouldn't");
          throw std::runtime_error("dynamic_cast<provingUpdateInterface*> fails, but based on component criterion it shouldn't");
        }
        // check if the UpdateInterface has been connected to a (controller) component. If so don't take over the control by adding it into updateOrder.
        auto connectionInfoUpdateInterface = std::dynamic_pointer_cast<ConnectionInfo<UpdateInterface>>(component);
        
        if (connectionInfoUpdateInterface->GetProvidedTo().size() == 0)
        {
          updateOrder.push_back(provingUpdateInterface);
          connectionInfoUpdateInterface->SetProvidedTo("NetworkBuilder");
        }

        // Before update should always be called
        beforeUpdateOrder.push_back(provingUpdateInterface);
      }
    }

    return NetworkContainer( components, beforeUpdateOrder, updateOrder, outputObjectsMap );
  }
  else
  {
    std::stringstream msg;
    msg << "Network is not realized yet";
    this->m_Logger.Log(LogLevel::ERR, "{}", msg.str() );
    throw std::runtime_error( msg.str() );
  }


}

template< typename ComponentList >
void
NetworkBuilder< ComponentList >::Cite()
{
  const BlueprintImpl::ComponentNamesType componentNames = m_Blueprint.GetComponentNames();
  for( auto const & componentName : componentNames ) {
    this->m_ComponentSelectorContainer[componentName]->GetComponent()->Cite();
  }
}

} // end namespace selx
