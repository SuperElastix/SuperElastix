#ifndef __Blueprint_h
#define __Blueprint_h

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/directed_graph.hpp"

#include "itkObjectFactory.h"
#include "itkDataObject.h"

#include "elxMacro.h"

namespace elx {

class Blueprint : public itk::DataObject
{
public:

  elxNewMacro( Blueprint, itk::DataObject );

  typedef std::string                                                ParameterKeyType;
  typedef std::vector< std::string >                                 ParameterValueType;
  typedef std::map< ParameterKeyType, ParameterValueType >           ParameterMapType;

  // Component parameter map that sits on a node in the graph 
  // and holds component configuration settings
  struct ComponentPropertyType { 
    ParameterMapType parameterMap;
  };

  // Component parameter map that sits on an edge in the graph 
  // and holds component configuration settings
  struct ConnectionPropertyType { 
    ParameterMapType parameterMap;
  };
  
  typedef boost::adjacency_list< boost::vecS,      
                                 boost::vecS,      
                                 boost::directedS,
                                 ComponentPropertyType,
                                 ConnectionPropertyType >             GraphType;

  typedef boost::graph_traits< GraphType >::vertex_descriptor         ComponentIndexType;
  typedef boost::graph_traits< GraphType >::vertex_iterator           ComponentIteratorType;
  typedef std::pair< ComponentIteratorType, ComponentIteratorType >   ComponentIteratorPairType;

  typedef boost::graph_traits< GraphType >::edge_descriptor           ConnectionIndexType;
  typedef boost::graph_traits< GraphType >::edge_iterator             ConnectionIteratorType;
  typedef std::pair< ConnectionIteratorType, ConnectionIteratorType > ConnectionIteratorPairType;

  typedef boost::graph_traits< GraphType >::in_edge_iterator          InputIteratorType;
  typedef std::pair< InputIteratorType, InputIteratorType >           InputIteratorPairType;

  typedef boost::graph_traits< GraphType >::out_edge_iterator         OutputIteratorType;
  typedef std::pair< OutputIteratorType, OutputIteratorType >         OutputIteratorPairType;

  // Interface for managing components
  ComponentIndexType AddComponent( void );
  ComponentIndexType AddComponent( ParameterMapType parameterMap );
  ParameterMapType GetComponent( ComponentIndexType index );
  bool SetComponent( ComponentIndexType, ParameterMapType parameterMap );
  void DeleteComponent( ComponentIndexType );
  bool ComponentExist( ComponentIndexType index );

  ComponentIteratorPairType GetComponentIterator( void ) {
    return boost::vertices(this->m_Graph);
  }


  // Returns the outgoing connections from a component in the graph,
  // i.e. all components that reads data from given component
  OutputIteratorPairType GetOuptutIterator( const ComponentIndexType index ) {
    return boost::out_edges(index, this->m_Graph);
  }

private:

  GraphType m_Graph;

};

}

#endif // #define __Blueprint_h