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

#ifndef selxBlueprintImpl_h
#define selxBlueprintImpl_h

#include "selxBlueprint.h"
#include "selxLoggerImpl.h"

#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/labeled_graph.hpp>

// for UpdateOrder
#include <boost/graph/topological_sort.hpp>

// for ComposeWith
#include <boost/graph/copy.hpp>

// for FromFile and MergeFromFile
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <boost/algorithm/string.hpp>

namespace selx
{

class BlueprintImpl
{
public:

  typedef std::shared_ptr< BlueprintImpl > BlueprintImplPointer;
  typedef Blueprint::ParameterKeyType ParameterKeyType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef Blueprint::ParameterMapType ParameterMapType;
  typedef Blueprint::ComponentNameType ComponentNameType;
  typedef Blueprint::ComponentNamesType ComponentNamesType;
  typedef Blueprint::ConnectionNameType ConnectionNameType;
  typedef Blueprint::ConnectionNamesType ConnectionNamesType;

  

  // Component parameter map that sits on a node in the graph
  // and holds component configuration settings
  struct ComponentPropertyType
  {
    ComponentPropertyType( ComponentNameType name = "", ParameterMapType parameterMap = {} ) : name( name ), parameterMap( parameterMap ) {}
    ComponentNameType name;
    ParameterMapType  parameterMap;
  };

  // Component parameter map that sits on an edge in the graph
  // and holds component connection configuration settings
  struct ConnectionPropertyType
  {
    ConnectionPropertyType( ConnectionNameType name = "", ParameterMapType parameterMap = {} ) :name( name ),  parameterMap( parameterMap ) {}
    ConnectionNameType name;
    ParameterMapType parameterMap;
  };

  typedef boost::labeled_graph<
    boost::adjacency_list<
    boost::multisetS, //OutEdgeList: allow multiple parallel connections
    boost::vecS, // VertexList, 
    boost::bidirectionalS, //Directed: directed graph, but give access to traverse in both directions
    ComponentPropertyType, //VertexProperties
    ConnectionPropertyType //GraphProperties
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

  explicit BlueprintImpl( LoggerImpl & loggerImpl);


  bool SetComponent( ComponentNameType, ParameterMapType parameterMap );

  ParameterMapType GetComponent( ComponentNameType componentName ) const;

  bool DeleteComponent( ComponentNameType componentName );

  bool ComponentExists( ComponentNameType componentName ) const;

  // Returns a vector of the all Component names in the graph.
  ComponentNamesType GetComponentNames( void ) const;

  // SetConnection with parameters parameterMap between components upstream and downstream. 
  // The connection name is to distinguish parallel connections. For single connections name = "" is typical. 
  bool SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap, ConnectionNameType name );

  ParameterMapType GetConnection( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name ) const;

  bool DeleteConnection( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name );

  bool ConnectionExists( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name ) const;

  bool ComposeWith( const BlueprintImpl & other );

  // Returns a vector of the Component names at the incoming direction
  ComponentNamesType GetInputNames( const ComponentNameType name ) const;

  // Returns a vector of the Component names at the outgoing direction
  ComponentNamesType GetOutputNames( const ComponentNameType name ) const;

  // Returns a vector of the connection names between upstream and downstream
  ComponentNamesType GetConnectionNames(const ComponentNameType upstream, const ComponentNameType downstream) const;

  ComponentNamesType GetUpdateOrder() const;

  void Write( const std::string filename );

  void MergeFromFile(const std::string & filename);

  void SetLoggerImpl( LoggerImpl & loggerImpl );

private:

  typedef boost::property_tree::ptree         PropertyTreeType;

  using PathType = boost::filesystem::path;
  using PathsType = std::list<PathType>;

  static PropertyTreeType ReadPropertyTree(const PathType & filename);

  static PathsType FindIncludes(const PropertyTreeType &);
  static ParameterValueType VectorizeValues(const PropertyTreeType& componentOrConnectionTree);

  Blueprint::Pointer FromPropertyTree(const PropertyTreeType &);
  void MergeProperties(const PropertyTreeType &);

  GraphType m_Graph;

  LoggerImpl * m_LoggerImpl;
};
} // namespace selx

#endif // #ifndef selxBlueprintImpl_h
