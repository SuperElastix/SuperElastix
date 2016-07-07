#include "selxMeta.h"

template<typename>
struct RegisterFactoriesByTypeList;

template < typename ... ComponentTypes>
struct RegisterFactoriesByTypeList < TypeList < ComponentTypes ... > > {
  static void Register()
  {
  }
};

template<typename ComponentType, typename ... Rest>
struct RegisterFactoriesByTypeList < TypeList<ComponentType, Rest ...> > {
  static void Register()
  {
    ComponentFactory<ComponentType>::RegisterOneFactory();
    RegisterFactoriesByTypeList<TypeList<Rest...>>::Register();
  }
};