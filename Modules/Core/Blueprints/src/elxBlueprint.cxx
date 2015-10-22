#ifndef __Blueprint_hxx
#define __Blueprint_hxx

#include <boost/graph/graphviz.hpp>

#include "elxBlueprint.h"

namespace elx {

Blueprint::ComponentIndexType
Blueprint
::AddComponent( void )
{
  this->Modified();

  // Create vertex
  ComponentIndexType index = boost::add_vertex( this->m_Graph );

  // Return component index so component can retrieved at a later time
  return index;
}

Blueprint::ComponentIndexType
Blueprint
::AddComponent( ParameterMapType parameterMap )
{
  this->Modified();

  // Create vertex
  ComponentIndexType index = boost::add_vertex( this->m_Graph );
  
  // Add parameter map to vertex
  this->m_Graph[index].parameterMap = parameterMap;

  // Return component index so component can retrieved at a later time
  return index;
}

Blueprint::ParameterMapType
Blueprint
::GetComponent( ComponentIndexType index )
{
  this->Modified();
  return this->m_Graph[index].parameterMap;
}

void
Blueprint
::SetComponent( ComponentIndexType index, ParameterMapType parameterMap )
{
  this->Modified();
  this->m_Graph[index].parameterMap = parameterMap;
}

/*
void
Blueprint
::RemoveComponent( ComponentIndexType component )
{
  this->Modified();
  clear_vertex(u, this->m_Graph);
  remove_vertex(u, this->m_Graph);
}

/*
void
Blueprint
::SetConnection( ComponentDescriptor upstream, ComponentDescriptor downstream )
{
  this->Modified();
}

void
Blueprint
::GetConnection( ConnectionDescriptorType Connection )
{
  this->Modified();
}

void 
Blueprint
::RemoveConnection( ConnectionDescriptorType connection )
{
  this->Modified();
}
*/

// Blueprint::ComponentDescriptorType
// Blueprint
// ::GetComponentDescriptor( ComponentIndexType componentIndex )
// {

// }

// void 
// Blueprint
// ::PrintGraph( void )
// {
//   // TODO: Link to graphviz library
//   boost::write_graphviz(std::cout, this->m_Graph);
// }

} // namespace elx 

#endif // __Blueprint_hxx