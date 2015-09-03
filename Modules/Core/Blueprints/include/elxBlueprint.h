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

  typedef TComponentDescriptor                                           ComponentDescriptorType;
  
  typedef boost::adjacency_list< boost::listS,      
                                 boost::listS,      
                                 boost::directedS,
                                 ComponentDescriptorType >               GraphType;

  typedef typename boost::graph_traits< GraphType >::vertex_descriptor   ComponentType;
  typedef typename boost::graph_traits< GraphType >::vertex_iterator     ComponentIterator, ComponentIteratorEnd;

  typedef typename boost::graph_traits< GraphType >::edge_descriptor     ConnectionDescriptorType;
  typedef typename boost::graph_traits< GraphType >::edge_iterator       ConnectionIterator, ConnectionIteratorEnd;

  typedef typename boost::graph_traits< GraphType >::in_edge_iterator    InputIterator, InputIteratorEnd;
  typedef typename boost::graph_traits< GraphType >::out_edge_iterator   OutputIterator, OutputIteratorEnd;

  void SetGraph( GraphType graph ) { this->m_Graph = graph; };
  GraphType GetGraph( void ) const { return m_Graph; };

  void AddComponent( ComponentDescriptorType component );
  void RemoveComponent( ComponentDescriptorType component );

  void AddConnection( ComponentDescriptorType upstream, ComponentDescriptorType downstream );
  void RemoveConnection( ConnectionDescriptorType connection );

private:

  GraphType m_Graph;

};

}

#endif // #define __Blueprint_h