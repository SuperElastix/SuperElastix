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

  typedef std::vector< std::string >                    ParameterValuesType;
  typedef std::map< std::string, ParameterValuesType >  ParameterMapType;
  typedef std::vector< ParameterMapType >               ParameterMapListType;

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
  
  ParameterMapListType GetParameterMapList( void ) 
  {
    return this->m_ParameterMapList; 
  };

  // TODO:
  // itkSetMacro( ParameterMap, ParameterMapType )
  // itkGetMacro( ParameterMap, ParameterMapType )

  // friend ITKCommon_EXPORT std::ostream& operator<<( std::ostream& os, const ParameterObject& parameterObject )
  // {
  //   os << parameterObject.m_ParameterMapList;
  //   return os;
  // }

private:

  ParameterMapListType m_ParameterMapList;

};

} // namespace elx

#endif // #define ParameterMap_h