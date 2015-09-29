#ifndef __Blueprint_h
#define __Blueprint_h

#include "itkObjectFactory.h"
#include "itkDataObject.h"

#include "elxMacro.h"
#include "elxComponentDescriptor.h"

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/directed_graph.hpp"

namespace elx {

template< class TComponentDescriptor >
class Blueprint : public itk::DataObject
{
public:

  elxNewMacro( Blueprint, itk::DataObject );

  typedef TComponentDescriptor                                                ComponentDescriptorType;
  typedef typename TComponentDescriptor::ComponentNameType                    ComponentNameType;
  
  typedef boost::adjacency_list< boost::listS,      
                                 boost::listS,      
                                 boost::directedS,
                                 ComponentDescriptorType >                    GraphType;

  typedef typename boost::graph_traits< GraphType >::vertex_descriptor        ComponentType;
  typedef typename boost::graph_traits< GraphType >::vertex_iterator          ComponentIterator, ComponentIteratorEnd;

  

  // TODO: Why can't we get the vertex index map type like they show in 
  // http://www.boost.org/doc/libs/1_38_0/libs/graph/doc/quick_tour.html
  // under "Access Vertex Set"?
  typedef boost::vertex_index_t                                               ComponentIndexType;
  typedef typename boost::property_map< GraphType, ComponentIndexType >::type ComponentIndexMapType;

  typedef typename boost::graph_traits< GraphType >::edge_descriptor          ConnectionDescriptorType;
  typedef typename boost::graph_traits< GraphType >::edge_iterator            ConnectionIterator, ConnectionIteratorEnd;

  typedef typename boost::graph_traits< GraphType >::in_edge_iterator         InputIterator, InputIteratorEnd;
  typedef typename boost::graph_traits< GraphType >::out_edge_iterator        OutputIterator, OutputIteratorEnd;

  bool AddComponent( ComponentDescriptorType component );
  // bool SetComponent( ComponentIndexType componentIndex, ComponentDescriptorType component );
  // ComponentDescriptorType GetComponent( ComponentIndexType componentIndex );

  // bool SetConnection( ComponentIndexType upstream, ComponentIndexType downstream );
  ConnectionDescriptorType GetConnection( ConnectionDescriptorType Connection );

private:

  GraphType m_Graph;

};

}

#endif // #define __Blueprint_h