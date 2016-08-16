#include "selxTypeList.h"

#ifndef RegisterByTypeList_h
#define RegisterByTypeList_h

namespace selx
{
template< typename >
struct RegisterFactoriesByTypeList;

template< typename ... ComponentTypes >
struct RegisterFactoriesByTypeList< TypeList< ComponentTypes ... >>
{
  static void Register()
  {
  }
};

template< typename ComponentType, typename ... Rest >
struct RegisterFactoriesByTypeList< TypeList< ComponentType, Rest ... >>
{
  static void Register()
  {
    ComponentFactory< ComponentType >::RegisterOneFactory();
    RegisterFactoriesByTypeList< TypeList< Rest ... >>::Register();
  }
};
}
#endif // RegisterByTypeList_h
