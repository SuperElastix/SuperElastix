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

#ifndef BlueprintImpl_h
#define BlueprintImpl_h

#include "boost/graph/graphviz.hpp"
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/directed_graph.hpp"
#include "boost/graph/labeled_graph.hpp"

#include "selxBlueprint.h"

namespace selx {
  
struct Blueprint::BlueprintImpl {

  // Component parameter map that sits on a node in the graph
  // and holds component configuration settings
  struct ComponentPropertyType
  {
    ComponentNameType name;
    ParameterMapType  parameterMap;
  };

  // Component parameter map that sits on an edge in the graph
  // and holds component connection configuration settings
  struct ConnectionPropertyType
  {
    ParameterMapType parameterMap;
  };

  typedef boost::labeled_graph< 
    boost::adjacency_list<
      boost::vecS,
      boost::vecS,
      boost::bidirectionalS,
      ComponentPropertyType,
      ConnectionPropertyType
    >,
    ComponentNameType > GraphType;
    
  typedef boost::graph_traits< GraphType >::vertex_descriptor       ComponentIndexType;
  typedef boost::graph_traits< GraphType >::vertex_iterator         ComponentIteratorType;
  typedef std::pair< ComponentIteratorType, ComponentIteratorType > ComponentIteratorPairType;

  typedef boost::graph_traits< GraphType >::edge_descriptor           ConnectionIndexType;
  typedef boost::graph_traits< GraphType >::edge_iterator             ConnectionIteratorType;
  typedef std::pair< ConnectionIteratorType, ConnectionIteratorType > ConnectionIteratorPairType;

  typedef boost::graph_traits< GraphType >::in_edge_iterator InputIteratorType;
  typedef std::pair< InputIteratorType, InputIteratorType >  InputIteratorPairType;

  typedef boost::graph_traits< GraphType >::out_edge_iterator OutputIteratorType;
  typedef std::pair< OutputIteratorType, OutputIteratorType > OutputIteratorPairType;

  bool SetComponent( ComponentNameType, ParameterMapType parameterMap );

  ParameterMapType GetComponent( ComponentNameType componentName ) const;

  bool DeleteComponent( ComponentNameType componentName );
  
  bool ComponentExists( ComponentNameType componentName ) const;

  // Returns a vector of the all Component names in the graph.
  ComponentNamesType GetComponentNames( void ) const;

  bool SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap );

  ParameterMapType GetConnection( ComponentNameType upstream, ComponentNameType downstream ) const;

  bool DeleteConnection( ComponentNameType upstream, ComponentNameType downstream );

  bool ConnectionExists( ComponentNameType upstream, ComponentNameType downstream ) const;

  // Returns a vector of the Component names at the incoming direction
  ComponentNamesType GetInputNames( const ComponentNameType name ) const;
  
  // Returns a vector of the Component names at the outgoing direction
  ComponentNamesType GetOutputNames( const ComponentNameType name ) const;

  void Write( const std::string filename );
  
  ConnectionIndexType GetConnectionIndex( ComponentNameType upsteam, ComponentNameType downstream ) const;
  
  GraphType m_Graph;
  
};

} // namespace selx

#endif // #ifndef BlueprintImpl_h