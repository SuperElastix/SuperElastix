#ifndef __Blueprint_hxx
#define __Blueprint_hxx

#include <boost/graph/graphviz.hpp>

#include "elxBlueprint.h"

namespace elx {

/*
Blueprint< ComponentDescriptor >::ComponentDescriptorType
Blueprint< ComponentDescriptor >
::AddComponent( ComponentDescriptorType component )
{
  // TODO: Check that the component is in the ComponentDatabase
  this->Modified();
  return this->m_Graph->add_vertex( component );
}

bool
Blueprint< ComponentDescriptor >
::SetComponent( ComponentDescriptorType component )
{
  this->Modified();
  return this->m_Graph->remove_vertex( connection );
}

bool
Blueprint< ComponentDescriptor >
::RemoveComponent( ComponentDescriptorType component )
{
  this->Modified();
  return this->m_Graph->remove_vertex( connection );
}

bool
Blueprint< ComponentDescriptor >
::AddConnection( ComponentDescriptorType upsteam, ComponentDescriptorType downstream )
{
  this->Modified();
  return this->m_Graph->add_edge( upstream, downstream );
}

ConnectionDescriptorType
Blueprint< ComponentDescriptor >
::GetConnection( ConnectionDescriptorType Connection )
{
  this->Modified();
}

void 
Blueprint< ComponentDescriptor >
::RemoveConnection( ConnectionType connection )
{
  this->Modified();
  this->m_Graph->remove_edge( connection );
}

void 
Blueprint< ComponentDescriptor >
::PrintGraph( void )
{
  // TODO: Link to graphviz library
  // boost::write_graphviz(std::cout, this->m_Graph);
  std::cout << "Printed graph" << std::endl;
}
*/

template<>
int
Blueprint< ComponentDescriptor >
::TestFunction( void )
{ return 0; }

} // namespace elx 

#endif // __Blueprint_hxx