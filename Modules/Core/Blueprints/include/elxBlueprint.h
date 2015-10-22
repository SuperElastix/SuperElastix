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

  struct ComponentPropertyType { 
    ParameterMapType parameterMap;
  };

  struct ConnectionPropertyType { 
    ParameterMapType parameterMap;
  };
  
  typedef boost::adjacency_list< boost::vecS,      
                                 boost::vecS,      
                                 boost::directedS,
                                 ComponentPropertyType,
                                 ConnectionPropertyType >            GraphType;

  typedef boost::graph_traits< GraphType >::vertex_descriptor        ComponentIndexType;
  typedef boost::graph_traits< GraphType >::vertex_iterator          ComponentIterator, ComponentIteratorEnd;

  typedef boost::graph_traits< GraphType >::edge_descriptor          ConnectionIndexType;
  typedef boost::graph_traits< GraphType >::edge_iterator            ConnectionIterator, ConnectionIteratorEnd;

  typedef boost::graph_traits< GraphType >::in_edge_iterator         InputIterator, InputIteratorEnd;
  typedef boost::graph_traits< GraphType >::out_edge_iterator        OutputIterator, OutputIteratorEnd;

  ComponentIndexType AddComponent( void );
  ComponentIndexType AddComponent( ParameterMapType parameterMap );
  ParameterMapType GetComponent( ComponentIndexType componentDescriptor );
  void SetComponent( ComponentIndexType, ParameterMapType parameterMap );

  const ParameterMapType& operator[]( ComponentIndexType index ) {
    return this->GetComponent( index );
  }

private:

  GraphType m_Graph;

};

}

#endif // #define __Blueprint_h