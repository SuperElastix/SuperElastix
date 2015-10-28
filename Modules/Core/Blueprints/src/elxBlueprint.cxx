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

  ComponentIndexType index = boost::add_vertex( this->m_Graph );

  // Return component index so component can retrieved at a later time
  return index;
}

Blueprint::ComponentIndexType
Blueprint
::AddComponent( ParameterMapType parameterMap )
{
  this->Modified();

  ComponentIndexType index = boost::add_vertex( this->m_Graph );
  this->m_Graph[index].parameterMap = parameterMap;

  // Return component index so component can retrieved at a later time
  return index;
}

Blueprint::ParameterMapType
Blueprint
::GetComponent( ComponentIndexType index )
{
  this->Modified();

  return this->m_Graph[ index ].parameterMap;
}

void
Blueprint
::SetComponent( ComponentIndexType index, ParameterMapType parameterMap )
{
  this->Modified();
  this->m_Graph[ index ].parameterMap = parameterMap;
}

// TODO: See explanation in elxBlueprint.h
// void
// Blueprint
// ::DeleteComponent( const ComponentIndexType index )
// {
//   this->Modified();
//
//   clear_vertex( index, this->m_Graph );
//   remove_vertex( index, this->m_Graph );
// }

bool
Blueprint
::AddConnection( ComponentIndexType upstream, ComponentIndexType downstream )
{
  this->Modified();

  if( this->ConnectionExists( upstream, downstream) ) {
    return false;
  }
  
  // Adds directed connection from upstream component to downstream component
  return boost::add_edge( upstream, downstream, this->m_Graph ).second;
}

bool
Blueprint
::AddConnection( ComponentIndexType upstream, ComponentIndexType downstream, ParameterMapType parameterMap )
{
  this->Modified();

  // If the connection does not exist, add the parameter map, otherwise don't do anything
  // because previous settings will be overwritten. If the user do want to overwrite 
  // current settings, she should use SetConnection() instead where the intent is explicit.  
  if( this->ConnectionExists( upstream, downstream) ) {
    return false;
  }

  this->m_Graph[ this->GetConnectionIndex( upstream, downstream ) ].parameterMap = parameterMap;
  return true;
}

Blueprint::ParameterMapType
Blueprint
::GetConnection( ComponentIndexType upstream, ComponentIndexType downstream )
{
  this->Modified();

  return this->m_Graph[ this->GetConnectionIndex( upstream, downstream ) ].parameterMap;
}

bool
Blueprint
::SetConnection( ComponentIndexType upstream, ComponentIndexType downstream, ParameterMapType parameterMap )
{
  this->Modified();

  if( !this->ConnectionExists( upstream, downstream ) ) {
    return this->AddConnection( upstream, downstream, parameterMap );
  } else {
    this->m_Graph[ this->GetConnectionIndex( upstream, downstream ) ].parameterMap = parameterMap;
    return true;
  }
}

bool
Blueprint
::DeleteConnection( ComponentIndexType upstream, ComponentIndexType downstream )
{
  this->Modified();

  if( this->ConnectionExists( upstream, downstream ) ) {
    this->m_Graph.remove_edge( this->GetConnectionIndex( upstream, downstream ) );
  }
  
  return !this->ConnectionExists( upstream, downstream );
}

bool
Blueprint
::ConnectionExists( ComponentIndexType upstream, ComponentIndexType downstream )
{
  return boost::edge( upstream, downstream, this->m_Graph).second;
}

Blueprint::ConnectionIndexType
Blueprint
::GetConnectionIndex( ComponentIndexType upstream, ComponentIndexType downstream )
{
  // This function is part of the internal API and should fail hard if we use it incorrectly
  if( !this->ConnectionExists( upstream, downstream ) ) {
    itkExceptionMacro( "Blueprint does not contain connection from component " << upstream << " to " << downstream );
  }
  
  return boost::edge( upstream, downstream, this->m_Graph).first;
}

// void 
// Blueprint
// ::PrintGraph( void )
// {
//   // TODO: Link to graphviz library
//   boost::write_graphviz(std::cout, this->m_Graph);
// }

} // namespace elx 

#endif // __Blueprint_hxx