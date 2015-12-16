#ifndef ParameterObject_h
#define ParameterObject_h

#include "itkObjectFactory.h"
#include "itkDataObject.h"

#include "elxMacro.h"

namespace selx {

class ParameterObject : public itk::DataObject
{
public:

  elxNewMacro( ParameterObject, itk::DataObject );

  typedef std::string                                       ParameterKeyType;
  typedef std::string                                       ParameterValueType;
  typedef std::vector< ParameterKeyType >                   ParameterVectorType;
  typedef std::map< ParameterKeyType, ParameterVectorType > ParameterMapType;
  typedef std::vector< ParameterMapType >                   ParameterMapListType;

  void SetParameterMap( ParameterMapType parameterMap ) 
  {
    ParameterMapListType parameterMapList;
    parameterMapList.push_back( parameterMap );
    this->SetParameterMapList( parameterMapList );
  }

  void SetParameterMapList( ParameterMapListType parameterMapList ) 
  { 
    this->m_ParameterMapList = parameterMapList; 
  };
  
  ParameterMapListType& GetParameterMapList( void ) 
  {
    this->Modified();
    return this->m_ParameterMapList; 
  };

  const ParameterMapListType& GetParameterMapList( void ) const
  {
    return this->m_ParameterMapList; 
  };

private:

  ParameterMapListType m_ParameterMapList;

};

} // namespace elx

#endif // #define ParameterMap_h