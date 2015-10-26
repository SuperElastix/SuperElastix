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

  if( this->ComponentExist( index ) ) {
    return this->m_Graph[index].parameterMap;
  } else {
    itkExceptionMacro( "Blueprint does not contain component with index " << index );
  }
}

bool
Blueprint
::SetComponent( ComponentIndexType index, ParameterMapType parameterMap )
{
  this->Modified();

  if( this->ComponentExist( index ) )
  {
    this->m_Graph[index].parameterMap = parameterMap;
  } else {
    itkExceptionMacro( "Blueprint does not contain component with index " << index )
  }
}

void
Blueprint
::DeleteComponent( const ComponentIndexType index )
{
  this->Modified();

  if( this->ComponentExist( index ) ) {
    clear_vertex( index, this->m_Graph );
    remove_vertex( index, this->m_Graph );
  }
}

bool
Blueprint
::ComponentExist( ComponentIndexType index )
{
  return boost::vertex( index, this->m_Graph ) != boost::graph_traits< GraphType >::null_vertex();
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