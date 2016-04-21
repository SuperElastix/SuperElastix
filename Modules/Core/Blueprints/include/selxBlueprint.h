#ifndef Blueprint_h
#define Blueprint_h

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/directed_graph.hpp"
#include "boost/graph/labeled_graph.hpp"

#include "itkObjectFactory.h"
#include "itkDataObject.h"

#include "selxMacro.h"

namespace selx {

class Blueprint : public itk::DataObject
{
public:

  selxNewMacro( Blueprint, itk::DataObject );

  typedef std::string                                                ParameterKeyType;
  typedef std::vector< std::string >                                 ParameterValueType;
  typedef std::map< ParameterKeyType, ParameterValueType >           ParameterMapType;
  
  typedef std::string                                                ComponentNameType;
  
    // Component parameter map that sits on a node in the graph 
  // and holds component configuration settings
  struct ComponentPropertyType { 
    ComponentNameType name;
    ParameterMapType parameterMap;
  };

  // Component parameter map that sits on an edge in the graph 
  // and holds component connection configuration settings
  struct ConnectionPropertyType { 
    ParameterMapType parameterMap;
  };
  
  typedef boost::labeled_graph < boost::adjacency_list <                       
                                                        boost::vecS,
                                                        boost::vecS,
                                                        boost::directedS,
                                                        ComponentPropertyType,
                                                        ConnectionPropertyType 
                                                        >,
                                 ComponentNameType >                  GraphType;
  
  //typedef GraphType::label_type                                       ComponentNameType;
  typedef std::vector<ComponentNameType>                              ComponentNamesType;

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
  bool AddComponent( ComponentNameType name);
  bool AddComponent(ComponentNameType name, ParameterMapType parameterMap);
  ParameterMapType GetComponent(ComponentNameType name);
  void SetComponent(ComponentNameType, ParameterMapType parameterMap);

  // TODO: Let user delete component. Before we do this, we need a proper way of 
  // checking that a vertex exist. Otherwise a call to GetComponent() on 
  // a deleted vertex will result in segfault. It is not really a in issue
  // _before_ release since typically we (the developers) will use blueprint 
  // interface procedurally.
  // void DeleteComponent( ComponentIndexType );

  ComponentNamesType GetComponentNames(void);

  ComponentIteratorPairType GetComponentIterator( void ) {
    return boost::vertices( this->m_Graph );
  }

  // Interface for managing connections between components in which we 
  // deliberately avoid using connection indexes, but instead force
  // the user to think in terms of components (which is conceptually simpler)
  bool AddConnection(ComponentNameType upstream, ComponentNameType downstream);
  bool AddConnection(ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap);
  ParameterMapType GetConnection(ComponentNameType upstream, ComponentNameType downstream);
  bool SetConnection(ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap);
  bool DeleteConnection(ComponentNameType upstream, ComponentNameType downstream);
  bool ConnectionExists(ComponentNameType upstream, ComponentNameType downstream);


  // TODO: can we really regard this as deprecated? Remove then.
  // Returns iterator for all connections in the graph
  //ConnectionIteratorPairType GetConnectionIterator(void);

  // Returns the outgoing connections from a component in the graph,
  // i.e. all components that reads data from given component
  //OutputIteratorPairType GetOutputIterator(const ComponentNameType name);

  // Returns a vector of the Component names at the outgoing direction
  // TODO: should this be an iterator over the names?
  ComponentNamesType GetOutputNames(const ComponentNameType name);

  void WriteBlueprint( const std::string filename );

private:
 
  ConnectionIndexType GetConnectionIndex( ComponentNameType upsteam, ComponentNameType downstream );

  GraphType m_Graph;

};

}

#endif // #define Blueprint_h