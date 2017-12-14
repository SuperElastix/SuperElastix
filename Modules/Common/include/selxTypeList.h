/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// Following the tutorial on http://pdimov.com/cpp2/simple_cxx11_metaprogramming.html

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
