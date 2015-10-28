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
  
  typedef boost::adjacency_list< boost::listS,      
                                 boost::listS,      
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
  void SetComponent( ComponentIndexType, ParameterMapType parameterMap );

  // TODO: Let user delete component. Before we do this, we need a proper way of 
  // checking that a vertex exist. Otherwise a call to  GetComponent() on 
  // a deleted vertex will result in segfault. It is not really a in issue
  // _before_ realease since typically we use (the developers) use blueprint 
  // interface procedurally.
  // void DeleteComponent( ComponentIndexType );

  ComponentIteratorPairType GetComponentIterator( void ) {
    return boost::vertices( this->m_Graph );
  }

  // Interface for managing connections between components in which we 
  // deliberately avoid using connection indexes, but instead force
  // the user to think in terms of components (which is conceptually simpler)
  bool AddConnection( ComponentIndexType upstream, ComponentIndexType downstream );
  bool AddConnection( ComponentIndexType upstream, ComponentIndexType downstream, ParameterMapType parameterMap );
  ParameterMapType GetConnection( ComponentIndexType upstream, ComponentIndexType downstream );
  bool SetConnection(  ComponentIndexType upstream, ComponentIndexType downstream, ParameterMapType parameterMap );
  bool DeleteConnection( ComponentIndexType upstream, ComponentIndexType downstream );
  bool ConnectionExists( ComponentIndexType upstream, ComponentIndexType downstream );

  // Returns iterator for all connections in the graph
  ConnectionIteratorPairType GetConnectionIterator( void ) {
    return boost::edges(this->m_Graph);
  }

  // Returns the outgoing connections from a component in the graph,
  // i.e. all components that reads data from given component
  OutputIteratorPairType GetOutputIterator( const ComponentIndexType index ) {
    return boost::out_edges(index, this->m_Graph);
  }

private:
  
  ConnectionIndexType GetConnectionIndex( ComponentIndexType upsteam, ComponentIndexType downstream );

  GraphType m_Graph;

};

}

#endif // #define __Blueprint_h