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


#ifdef _MSC_VER
#  if _MSC_VER == 1916 // Visual Studio 2017 version 15.9
#    ifdef _PREFAST_ // Defined as 1 when the /analyze compiler option is set.
// Warning C26489 often appears as a false positive:
// https://developercommunity.visualstudio.com/content/problem/469978/c-code-analysis-false-positives-c26489-on-stdmap.html
#pragma warning(disable: 26489) // Don't dereference a pointer that may be invalid: '...' may have been invalidated at line ... (lifetime.1).
#    endif
#  endif
#endif


#include "selxBlueprintImpl.h"
#include "selxLoggerImpl.h"

// Standard C++ Library headers:
#include <ostream>
#include <stdexcept>
#include <tuple>  // For std::tie.


namespace selx
{
// Declared outside of the class body, so it is a free function
std::ostream &
operator<<( std::ostream & out, const BlueprintImpl::ParameterMapType & val )
{
  for( auto const & mapPair : val )
  {
    out << mapPair.first << " : [ ";
    for( auto const & value : mapPair.second )
    {
      out << value << " ";
    }
    out << "]\\n";
  }
  return out;
}


template< class NameType, class ParameterMapType >
class vertex_label_writer
{
public:

  vertex_label_writer( NameType _name, ParameterMapType _parameterMap ) : name( _name ), parameterMap( _parameterMap ) {}
  template< class VertexOrEdge >
  void operator()( std::ostream & out, const VertexOrEdge & v ) const
  {
    out << "[label=\"" << name[ v ] << "\n" << parameterMap[ v ] << "\"]";
  }


private:

  NameType         name;
  ParameterMapType parameterMap;
};

template< class NameType, class ParameterMapType >
inline vertex_label_writer< NameType, ParameterMapType >
make_vertex_label_writer( NameType n, ParameterMapType p )
{
  return vertex_label_writer< NameType, ParameterMapType >( n, p );
}


template< class ParameterMapType >
class edge_label_writer
{
public:

  edge_label_writer( ParameterMapType _parameterMap ) : parameterMap( _parameterMap ) {}
  template< class VertexOrEdge >
  void operator()( std::ostream & out, const VertexOrEdge & v ) const
  {
    out << "[label=\"" << parameterMap[ v ] << "\"]";
  }


private:

  ParameterMapType parameterMap;
};

template< class ParameterMapType >
inline edge_label_writer< ParameterMapType >
make_edge_label_writer( ParameterMapType p )
{
  return edge_label_writer< ParameterMapType >( p );
}


BlueprintImpl::BlueprintImpl( LoggerImpl & loggerImpl ) : m_LoggerImpl(&loggerImpl)
{
}


bool
BlueprintImpl
::SetComponent( ComponentNameType name, ParameterMapType parameterMap )
{
  if( this->ComponentExists( name ) )
  {
    this->m_Graph[ name ].parameterMap = parameterMap;
    return true;
  }
  else
  {
    return this->m_Graph.insert_vertex( name, { name, parameterMap } ).second;
  }
}


BlueprintImpl::ParameterMapType
BlueprintImpl
::GetComponent( ComponentNameType name ) const
{
  if( !this->ComponentExists( name ) )
  {
    std::stringstream msg;
    msg << "BlueprintImpl does not contain component " << name << std::endl;
    this->m_LoggerImpl->Log(LogLevel::CRT, msg.str());
    throw std::runtime_error( msg.str() );
  }

  return this->m_Graph[ name ].parameterMap;
}


bool
BlueprintImpl
::DeleteComponent( ComponentNameType name )
{
  if( !this->ComponentExists( name ) )
  {
    this->m_Graph.remove_vertex( name );
    return true;
  }

  return false;
}


BlueprintImpl::ComponentNamesType
BlueprintImpl
::GetComponentNames() const
{
  ComponentNamesType container;
  for( auto it = boost::vertices( this->m_Graph.graph() ).first; it != boost::vertices( this->m_Graph.graph() ).second; ++it )
  {
    container.push_back( this->m_Graph.graph()[ *it ].name );
  }
  return container;
}


bool
BlueprintImpl
::SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap, ConnectionNameType name )
{
  if( !this->ComponentExists( upstream ) || !this->ComponentExists( downstream ) )
  {
    this->m_LoggerImpl->Log(LogLevel::WRN, "Setting a connection between components '{}' and '{}' failed: one or more components do not exist", upstream, downstream);
    return false;
  }

  // Multiple parallel connections are allowed. If a connection with the name "name" exists it should be overridden, otherwise just added.
    
  boost::graph_traits<GraphType>::out_edge_iterator ei, ei_end;
  // too bad edge_range_by_label doesn't exist
  std::tie(ei, ei_end) = boost::edge_range(this->m_Graph.vertex(upstream), this->m_Graph.vertex(downstream), this->m_Graph.graph());

  for (; ei != ei_end; ++ei) 
  {
    auto existingName = boost::get(&ConnectionPropertyType::name, this->m_Graph.graph(), *ei);
    if (name == existingName)
    {
      // override previous parameterMap
      boost::put(&ConnectionPropertyType::parameterMap, this->m_Graph.graph(), *ei, parameterMap);
      return true;
    }
  }// no existing connections named "name" were found.
 
  boost::add_edge_by_label(upstream, downstream, { name, parameterMap }, this->m_Graph);
  return true;
}


BlueprintImpl::ParameterMapType
BlueprintImpl
::GetConnection( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name ) const
{
  // This function assumes the connection exists.

  boost::graph_traits<GraphType>::out_edge_iterator ei, ei_end;
  // too bad edge_range_by_label doesn't exist
  std::tie(ei, ei_end) = boost::edge_range(this->m_Graph.vertex(upstream), this->m_Graph.vertex(downstream), this->m_Graph.graph());

  for (; ei != ei_end; ++ei)
  {
    auto existingName = boost::get(&ConnectionPropertyType::name, this->m_Graph.graph(), *ei);
    if (name == existingName)
    {
      return boost::get(&ConnectionPropertyType::parameterMap, this->m_Graph.graph(), *ei);
    }
  } // no existing connections named "name" were found.
  
  throw std::runtime_error( "BlueprintImpl does not contain connection from component " + upstream + " to " + downstream + " by name " + name );
} 

bool
BlueprintImpl
::DeleteConnection( BlueprintImpl::ComponentNameType upstream, BlueprintImpl::ComponentNameType downstream, ConnectionNameType name )
{
  if( this->ConnectionExists( upstream, downstream, name ) )
  {
    boost::graph_traits<GraphType>::out_edge_iterator ei, ei_end;
    // too bad edge_range_by_label doesn't exist
    std::tie(ei, ei_end) = boost::edge_range(this->m_Graph.vertex(upstream), this->m_Graph.vertex(downstream), this->m_Graph.graph());
    for (; ei != ei_end; ++ei) {
      auto existingName = boost::get(&ConnectionPropertyType::name, this->m_Graph.graph(), *ei);
      if (name == existingName)
      {
        boost::remove_edge(*ei, this->m_Graph.graph());
        return true;
      }
    }
  }

  return false;
}


bool
BlueprintImpl
::ComponentExists( ComponentNameType componentName ) const
{
  return this->m_Graph.vertex( componentName ) != boost::graph_traits< GraphType >::null_vertex();
}


bool
BlueprintImpl
::ConnectionExists( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name ) const
{
  if( !this->ComponentExists( upstream ) )
  {
    return false;
  }
  if( !this->ComponentExists( downstream ) )
  {
    return false;
  }

  if (boost::edge_by_label( upstream, downstream, this->m_Graph ).second)
  {
    boost::graph_traits<GraphType>::out_edge_iterator ei, ei_end;
    // too bad edge_range_by_label doesn't exist
    std::tie(ei, ei_end) = boost::edge_range(this->m_Graph.vertex(upstream), this->m_Graph.vertex(downstream), this->m_Graph.graph());

    for (; ei != ei_end; ++ei)
    {
      auto existingName = boost::get(&ConnectionPropertyType::name, this->m_Graph.graph(), *ei);
      if (name == existingName)
      {
        return true;
      }
    }
  } // no existing connections named "name" were found.

  return false;
}


bool
BlueprintImpl
::ComposeWith( const BlueprintImpl & other )
{
  // Make a backup of the current blueprint status in case composition fails
  GraphType graph_backup = GraphType( this->m_Graph );

  // Copy-in all components (Nodes)
  for( auto const & componentName : other.GetComponentNames() )
  {
    // Does other blueprint use component with a name that already exists?
    if( this->ComponentExists( componentName ) )
    {
      // Component exists, check if properties can be merged
      auto ownProperties    = this->GetComponent( componentName );
      auto othersProperties = other.GetComponent( componentName );

      for( auto const & othersEntry : othersProperties )
      {
        const ParameterMapType::const_iterator foundProperty = ownProperties.find(othersEntry.first);

        // Does other use a property key that already exists in this component?
        if (foundProperty != ownProperties.cend())
        {
          // Are the property values equal?
          if (foundProperty->second != othersEntry.second)
          {
            // No, Blueprints cannot be Composed
            this->m_Graph = graph_backup;
            return false;
          }
        }
        else
        {
          // Property key doesn't exist yet, add entry to this component
          ownProperties[ othersEntry.first ] = othersEntry.second;
          this->SetComponent( componentName, ownProperties );
        }
      }
    }
    else
    {
      // Create Component copying properties of other
      this->SetComponent( componentName, other.GetComponent( componentName ) );
    }
  }
  // Copy-in all connections (Edges)
  for( auto const & componentName : other.GetComponentNames() )
  {
    for( const auto& incomingName : other.GetInputNames( componentName ) )
    {
      for (const auto& connectionName : other.GetConnectionNames(incomingName, componentName ) )
      {
        // Does other blueprint have a connection that already exists?
        if (this->ConnectionExists(incomingName, componentName, connectionName ))
        {
          // Connection exists, check if properties can be merged
          auto ownProperties    = this->GetConnection( incomingName, componentName, connectionName );
          auto othersProperties = other.GetConnection( incomingName, componentName, connectionName );

          for( auto const & othersEntry : othersProperties )
          {
            const ParameterMapType::const_iterator foundProperty = ownProperties.find(othersEntry.first);

            // Does other use a property key that already exists in this component?
            if (foundProperty != ownProperties.cend())
            {
              // Are the property values equal?
              if (foundProperty->second != othersEntry.second)
              {
                // No, Blueprints cannot be Composed
                this->m_Graph = graph_backup;
                return false;
              }
            }
            else
            {
              // Property key doesn't exist yet, add entry to this component
              ownProperties[ othersEntry.first ] = othersEntry.second;
              this->SetConnection( incomingName, componentName, ownProperties, connectionName );
            }
          } // end loop otherProperties
        }
        else // connection with connectionName does not exist
        {
          // Create Connection copying properties of other
          this->SetConnection( incomingName, componentName, other.GetConnection( incomingName, componentName, connectionName ),  connectionName );
        }
      }
    }
  }

  return true;
}

BlueprintImpl::ComponentNamesType
BlueprintImpl
::GetInputNames( const ComponentNameType name ) const
{
  ComponentNamesType container;
  //auto vertex = this->m_Graph.vertex(name);
  //boost::in_edges(vertex, this->m_Graph);
  InputIteratorPairType inputIteratorPair = boost::in_edges( this->m_Graph.vertex( name ), this->m_Graph );
  for( auto it = inputIteratorPair.first; it != inputIteratorPair.second; ++it )
  {
    container.push_back( this->m_Graph.graph()[ it->m_source ].name );
  }

  return container;
}


BlueprintImpl::ComponentNamesType
BlueprintImpl
::GetOutputNames( const ComponentNameType name ) const
{
  ComponentNamesType     container;
  OutputIteratorPairType outputIteratorPair = boost::out_edges( this->m_Graph.vertex( name ), this->m_Graph );
  for( auto it = outputIteratorPair.first; it != outputIteratorPair.second; ++it )
  {
    container.push_back( this->m_Graph.graph()[ it->m_target ].name );
  }

  return container;
}

BlueprintImpl::ComponentNamesType
BlueprintImpl
::GetUpdateOrder() const
{
  ComponentNamesType     container;
 
  std::vector< ComponentIndexType > indexContainer;
  
  boost::topological_sort(this->m_Graph, std::back_inserter(indexContainer));

  for (auto ii = indexContainer.rbegin(); ii != indexContainer.rend(); ++ii)
  {
    container.push_back(boost::get(boost::vertex_all, this->m_Graph, *ii).name);
  }
  return container;
}


BlueprintImpl::ConnectionNamesType
BlueprintImpl
::GetConnectionNames(const ComponentNameType upstream, const ComponentNameType downstream) const
{
  ConnectionNamesType     container;

  boost::graph_traits<GraphType>::out_edge_iterator ei, ei_end;
  // too bad edge_range_by_label doesn't exist
  std::tie(ei, ei_end) = boost::edge_range(this->m_Graph.vertex(upstream), this->m_Graph.vertex(downstream), this->m_Graph.graph());

  for (; ei != ei_end; ++ei)
  {
    container.push_back(boost::get(&ConnectionPropertyType::name, this->m_Graph.graph(), *ei));
  }
  return container;
}



void
BlueprintImpl
::Write( const std::string filename )
{
  std::ofstream dotfile( filename.c_str() );
  boost::write_graphviz( dotfile, this->m_Graph,
    make_vertex_label_writer( boost::get( &ComponentPropertyType::name, this->m_Graph ),
    boost::get( &ComponentPropertyType::parameterMap, this->m_Graph ) ),
    make_edge_label_writer( boost::get( &ConnectionPropertyType::parameterMap, this->m_Graph ) ) );
}

BlueprintImpl::ParameterValueType
BlueprintImpl::VectorizeValues(const PropertyTreeType& componentOrConnectionTree)
{
  std::string        propertySingleValue = componentOrConnectionTree.data();
  ParameterValueType propertyMultiValue;
  for (auto const & value : componentOrConnectionTree)
  {
    propertyMultiValue.push_back(value.second.data());
  }
  if (!propertyMultiValue.empty())
  {
    if (!propertySingleValue.empty())
    {
      throw std::invalid_argument("XML tree should have either 1 unnamed element or multiple named properties");
    }
  }
  else
  {
    propertyMultiValue.push_back(propertySingleValue);
  }
  return propertyMultiValue;
}

void
BlueprintImpl::MergeFromFile(const std::string & fileNameString)
{
  const PathType fileName(fileNameString);
  const auto parentPath = fileName.parent_path();

  this->m_LoggerImpl->Log(LogLevel::INF, "Loading {0} ... ", fileName);
  const auto propertyTree = ReadPropertyTree(fileName);
  this->m_LoggerImpl->Log(LogLevel::INF, "Loading {0} ... Done", fileName);

  this->m_LoggerImpl->Log(LogLevel::INF, "Checking {0} for include files ... ", fileName);

  for (auto const & includePath : FindIncludes(propertyTree))
  {
    this->m_LoggerImpl->Log(LogLevel::INF, "Including file {0} ... ", includePath);

    if (includePath.is_relative() && ! parentPath.empty())
    {
      const auto absoluteIncludePath = parentPath / includePath;
      this->MergeFromFile(absoluteIncludePath.string());
    }
    else
    {
      this->MergeFromFile(includePath.string());
    }
  }
  this->m_LoggerImpl->Log(LogLevel::INF, "Checking {0} for include files ... done", fileName);
  this->MergeProperties(propertyTree);
}

void
BlueprintImpl::SetLoggerImpl( LoggerImpl & loggerImpl )
{
  this->m_LoggerImpl = &loggerImpl;
}

BlueprintImpl::PropertyTreeType
BlueprintImpl::ReadPropertyTree(const PathType & filename)
{
  // Create empty property tree object
  PropertyTreeType propertyTree;
  if (filename.extension() == ".xml")
  {
    read_xml(filename.string(), propertyTree, boost::property_tree::xml_parser::trim_whitespace);
  }
  else if (filename.extension() == ".json")
  {
    read_json(filename.string(), propertyTree);
  }
  else
  {
    throw std::invalid_argument("Configuration file requires extension .xml or .json");
  }

  return propertyTree;
}

BlueprintImpl::PathsType
BlueprintImpl::FindIncludes(const PropertyTreeType & propertyTree)
{
  PathsType paths;
  const auto iteratorPair = propertyTree.equal_range("Include");

  if (iteratorPair.first != iteratorPair.second)
  {
    if (std::next(iteratorPair.first) != iteratorPair.second)
    {
      throw std::runtime_error("Only 1 listing of Includes is allowed per Blueprint file");
    }
    const PropertyTreeType::value_type & v = *iteratorPair.first;
    auto const pathsStrings = VectorizeValues(v.second);
    // convert vector of strings to list of boost::path-s
    paths = PathsType(pathsStrings.cbegin(), pathsStrings.cend());
  }
  return paths;
}


Blueprint::Pointer
BlueprintImpl::FromPropertyTree(const PropertyTreeType & pt)
{
  Blueprint::Pointer blueprint = Blueprint::New();

  for(const PropertyTreeType::value_type & v: pt.get_child("Components"))
  {
    std::string      componentName;
    ParameterMapType componentPropertyMap;
    for (auto const & elm : v.second)
    {
      const std::string & componentKey = elm.first;
      if (componentKey == "Name")
      {
        componentName = elm.second.data();
        continue;
      }

      ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
      std::string        propertyKey = elm.first;
      componentPropertyMap[propertyKey] = propertyMultiValue;
    }

    blueprint->SetComponent(componentName, componentPropertyMap);
  }

  const boost::optional< const PropertyTreeType& > connections_exist = pt.get_child_optional( "Connections" );
  if(connections_exist) {
    for(const PropertyTreeType::value_type & v: pt.get_child("Connections"))
    {
      std::string connectionName = v.second.data();
      if (!connectionName.empty())
      {
        this->m_LoggerImpl->Log(LogLevel::TRC, "Found {0}, but connection names are ignored.", connectionName);
      }
      std::string      outName;
      std::string      inName;
      ParameterMapType componentPropertyMap;

      for (auto const & elm : v.second)
      {
        const std::string & connectionKey = elm.first;

        if (connectionKey == "Out")
        {
          outName = elm.second.data();
          continue;
        }
        else if (connectionKey == "In")
        {
          inName = elm.second.data();
          continue;
        }
        else if (connectionKey == "Name")
        {
          this->m_LoggerImpl->Log(LogLevel::WRN, "Connections with key 'Name' are ignored.");
          continue;
        }
        else
        {
          ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
          std::string        propertyKey = elm.first;
          componentPropertyMap[propertyKey] = propertyMultiValue;
        }
      }

      blueprint->SetConnection(outName, inName, componentPropertyMap);
    }
  }
  return blueprint;
}

void
BlueprintImpl::MergeProperties(const PropertyTreeType & pt)
{
  for(const PropertyTreeType::value_type & v: pt.get_child("Components"))
  {
    std::string      componentName;
    ParameterMapType newProperties;
    for (auto const & elm : v.second)
    {
      const std::string & componentKey = elm.first;
      if (componentKey == "Name")
      {
        componentName = elm.second.data();
        continue;
      }

      ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
      std::string        propertyKey = elm.first;
      newProperties[propertyKey] = propertyMultiValue;
    }

    // Merge newProperies into current blueprint properties
    // Does blueprint use component with a name that already exists?
    if (this->ComponentExists(componentName))
    {
      // Component exists, check if properties can be merged
      auto ownProperties = this->GetComponent(componentName);

      for (auto const & othersEntry : newProperties)
      {
        const ParameterMapType::const_iterator foundProperty = ownProperties.find(othersEntry.first);

        // Does other use a property key that already exists in this component?
        if (foundProperty != ownProperties.cend())
        {
          // Are the property values equal?
          if (foundProperty->second != othersEntry.second)
          {
            // No, Blueprints cannot be Composed
            this->m_LoggerImpl->Log(LogLevel::ERR, "Merging blueprints failed : Component properties cannot be redefined");
            throw std::invalid_argument("Merging blueprints failed : Component properties cannot be redefined");
          }
        }
        else
        {
          // Property key doesn't exist yet, add entry to this component
          ownProperties[othersEntry.first] = othersEntry.second;
          this->SetComponent(componentName, ownProperties);
        }
      }
    }
    else
    {
      // Create Component and with the new properties
      this->SetComponent(componentName, newProperties);
    }
  }

  const boost::optional< const PropertyTreeType& > connections_exist = pt.get_child_optional( "Connections" );
  if(connections_exist) {
    for(const PropertyTreeType::value_type & v: pt.get_child("Connections"))
    {
      std::string connectionName = v.second.data();

      std::string outName;
      std::string inName;
      ParameterMapType newProperties;

      for (auto const &elm : v.second) {
        const std::string &connectionKey = elm.first;

        if (connectionKey == "Out") {
          outName = elm.second.data();
          continue;
        } else if (connectionKey == "In") {
          inName = elm.second.data();
          continue;
        } else if (connectionKey == "Name") {
          if (!connectionName.empty()) {
            this->m_LoggerImpl->Log(LogLevel::WRN, "Connection Name '{}' is overridden by '{}'", connectionName,
                                    elm.second.data());
          }
          connectionName = elm.second.data();
          continue;
        } else {
          ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
          std::string propertyKey = elm.first;
          newProperties[propertyKey] = propertyMultiValue;
        }
      }

      // Does the blueprint have a connection that already exists?
      if (this->ConnectionExists(outName, inName, connectionName)) {
        // Connection exists, check if properties can be merged
        auto ownProperties = this->GetConnection(outName, inName, connectionName);

        for (auto const &othersEntry : newProperties)
        {
          const ParameterMapType::const_iterator foundProperty = ownProperties.find(othersEntry.first);

          // Does newProperties use a key that already exists in this component?
          if (foundProperty != ownProperties.cend())
          {
            // Are the property values equal?
            if (foundProperty->second != othersEntry.second)
            {
              // No, Blueprints cannot be Composed
              this->m_LoggerImpl->Log(LogLevel::ERR, "Merging blueprints failed: Connection property {0} redefined.",
                                      othersEntry.first);
              throw std::invalid_argument("Merging blueprints failed: Connection properties cannot be redefined.");
            }
          } else {
            // Property key doesn't exist yet, add entry to this component
            ownProperties[othersEntry.first] = othersEntry.second;
            this->SetConnection(outName, inName, ownProperties, connectionName);
          }
        }
      } else {
        // Create Component copying properties of other
        this->SetConnection(outName, inName, newProperties, connectionName);
      }
    }
  }
}
} // namespace selx
