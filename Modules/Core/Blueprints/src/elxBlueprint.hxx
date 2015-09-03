#ifndef __Blueprint_hxx
#define __Blueprint_hxx

#include <boost/graph/graphviz.hpp>

#include "elxBlueprint.h"

namespace elastix {

void
Blueprint< ComponentDescriptor >
::Blueprint( void )
{
  this->m_Graph = GraphType;
}

ComponentDescriptorType
Blueprint< ComponentDescriptor >
::AddComponent( ComponentDescriptorType component )
{
  this->Modified();
  return this->m_Graph->add_vertex( component );
}

void 
Blueprint< ComponentDescriptor >
::RemoveComponent( ComponentDescriptorType component )
{
  this->Modified();
  this->m_Graph->remove_vertex( connection );
}

void
Blueprint< ComponentDescriptor >
::AddConnection( ComponentDescriptorType upsteam, ComponentDescriptorType downstream )
{
  this->Modified();
  this->m_Graph->add_edge( upstream, downstream );
}

void 
Blueprint< ComponentDescriptor >
::RemoveConnection( ConnectionType connection )
{
  this->Modified();
  this->m_Graph->remove_edge( connection );
}

void Blueprint< ComponentDescriptor >
::PrintSelf( void )
{
  boost::write_graphviz(std::cout, this->m_Graph);
}

}

#endif // __Blueprint_hxx