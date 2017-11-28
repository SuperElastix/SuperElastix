#ifndef selxTypeList_h
#define selxTypeList_h

namespace selx
{
template< typename ... >
struct TypeList;

template<class... L> struct list_append_impl;

template<class... L> using list_append = typename list_append_impl<L...>::type;

template<> struct list_append_impl<>
{
  using type = TypeList<>;
};

template<template<class...> class L, class... T> struct list_append_impl<L<T...>>
{
  using type = L<T...>;
};

template<template<class...> class L1, class... T1,
  template<class...> class L2, class... T2, class... Lr>
struct list_append_impl<L1<T1...>, L2<T2...>, Lr...>
{
  using type = list_append<L1<T1..., T2...>, Lr...>;
};

}
#endif // selxTypeList_h
