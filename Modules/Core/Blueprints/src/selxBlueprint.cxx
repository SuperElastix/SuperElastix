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

#ifndef Blueprint_cxx
#define Blueprint_cxx

#include "boost/graph/graphviz.hpp"

#include "selxBlueprint.h"

namespace selx
{
// Declared outside of the class body, so it is a free function
std::ostream &
operator<<( std::ostream & out, const Blueprint::ParameterMapType & val )
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


bool
Blueprint
::AddComponent( ComponentNameType name )
{
  this->Modified();

  // Returns true is addition was successful
  return this->m_Graph.insert_vertex( name, { name, { {} } } ).second;
}


bool
Blueprint
::AddComponent( ComponentNameType name, ParameterMapType parameterMap )
{
  this->Modified();

  // Returns true is addition was successful
  return this->m_Graph.insert_vertex( name, { name, parameterMap } ).second;
}


Blueprint::ParameterMapType
Blueprint
::GetComponent( ComponentNameType name ) const
{
  if( !this->ComponentExists( name ) )
  {
    itkExceptionMacro( "Blueprint does not contain component " << name );
  }
  this->Modified();
  return this->m_Graph[ name ].parameterMap;
}


void
Blueprint
::SetComponent( ComponentNameType name, ParameterMapType parameterMap )
{
  this->Modified();
  this->m_Graph[ name ].parameterMap = parameterMap;
}


// TODO: See explanation in selxBlueprint.h
// void
// Blueprint
// ::DeleteComponent( const ComponentIndexType index )
// {
//   this->Modified();
//
//   clear_vertex( index, this->m_Graph );
//   remove_vertex( index, this->m_Graph );
// }

Blueprint::ComponentNamesType
Blueprint::GetComponentNames( void ) const
{
  ComponentNamesType container;
  for( auto it = boost::vertices( this->m_Graph.graph() ).first; it != boost::vertices( this->m_Graph.graph() ).second; ++it )
  {
    container.push_back( this->m_Graph.graph()[ *it ].name );
  }
  return container;
}


bool
Blueprint
::AddConnection( ComponentNameType upstream, ComponentNameType downstream )
{
  this->Modified();

  if( this->ConnectionExists( upstream, downstream ) )
  {
    return false;
  }

  // Adds directed connection from upstream component to downstream component
  return boost::add_edge_by_label( upstream, downstream, this->m_Graph ).second;
}


bool
Blueprint
::AddConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap )
{
  this->Modified();

  if( !this->ConnectionExists( upstream, downstream ) )
  {
    // TODO check if vertices exist
    boost::add_edge_by_label( upstream, downstream,  { parameterMap }, this->m_Graph );
    return true;
  }

  // If the connection does not exist don't do anything because previous settings
  // will be overwritten.  If the user do want to overwrite current settings,
  // she should use SetConnection() instead where the intent is explicit.
  return false;
}


Blueprint::ParameterMapType
Blueprint
::GetConnection( ComponentNameType upstream, ComponentNameType downstream ) const
{
  this->Modified();

  return this->m_Graph[ this->GetConnectionIndex( upstream, downstream ) ].parameterMap;
}


bool
Blueprint
::SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap )
{
  this->Modified();

  if( !this->ConnectionExists( upstream, downstream ) )
  {
    return this->AddConnection( upstream, downstream, parameterMap );
  }
  else
  {
    this->m_Graph[ this->GetConnectionIndex( upstream, downstream ) ].parameterMap = parameterMap;
    return true;
  }
}


bool
Blueprint
::DeleteConnection( ComponentNameType upstream, ComponentNameType downstream )
{
  this->Modified();

  if( this->ConnectionExists( upstream, downstream ) )
  {
    boost::remove_edge_by_label( upstream, downstream, this->m_Graph );
  }

  return !this->ConnectionExists( upstream, downstream );
}


bool
Blueprint
::ComponentExists( ComponentNameType componentName ) const
{
  const GraphType::vertex_descriptor descriptor = this->m_Graph.vertex( componentName );
  return descriptor != boost::graph_traits< GraphType >::null_vertex();
}


bool
Blueprint
::ConnectionExists( ComponentNameType upstream, ComponentNameType downstream ) const
{
  if( !this->ComponentExists( upstream ) )
  {
    itkExceptionMacro( "Blueprint does not contain component " << upstream );
  }
  if( !this->ComponentExists( downstream ) )
  {
    itkExceptionMacro( "Blueprint does not contain component " << downstream );
  }

  return boost::edge_by_label( upstream, downstream, this->m_Graph ).second;
}


Blueprint::ComponentNamesType
Blueprint
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

Blueprint::ComponentNamesType
Blueprint
::GetInputNames(const ComponentNameType name) const
{
  ComponentNamesType    container;
  //auto vertex = this->m_Graph.vertex(name);
  //boost::in_edges(vertex, this->m_Graph);
  InputIteratorPairType inputIteratorPair = boost::in_edges(this->m_Graph.vertex(name), this->m_Graph);
  for (auto it = inputIteratorPair.first; it != inputIteratorPair.second; ++it)
  {
    container.push_back(this->m_Graph.graph()[it->m_source].name);
  }

  return container;
}

Blueprint::ConnectionIndexType
Blueprint
::GetConnectionIndex( ComponentNameType upstream, ComponentNameType downstream ) const
{
  // This function is part of the internal API and should fail hard if we use it incorrectly
  if( !this->ConnectionExists( upstream, downstream ) )
  {
    itkExceptionMacro( "Blueprint does not contain connection from component " << upstream << " to " << downstream );
  }

  return boost::edge_by_label( upstream, downstream, this->m_Graph ).first;
}


void
Blueprint
::WriteBlueprint( const std::string filename )
{
  std::ofstream dotfile( filename.c_str() );
  boost::write_graphviz( dotfile, this->m_Graph,
    make_vertex_label_writer( boost::get( &ComponentPropertyType::name, this->m_Graph ),
    boost::get( &ComponentPropertyType::parameterMap, this->m_Graph ) ),
    make_edge_label_writer( boost::get( &ConnectionPropertyType::parameterMap, this->m_Graph ) ) );
}
} // namespace selx

#endif // Blueprint_cxx
