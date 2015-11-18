#ifndef InterfaceTraits_h
#define InterfaceTraits_h

#include "Interfaces.h"

namespace elx
{
// Traits to get printable interface name
// default implementation
template <typename T>
struct InterfaceName
{
  static const char* Get()
  {
    return typeid(T).name();
  }
};

// a specialization for each type of those you want to support
// and don't like the string returned by typeid
template <>
struct InterfaceName < MetricValueInterface >
{
  static const char* Get()
  {
    return "MetricValueInterface";
  }
};
template <>
struct InterfaceName < MetricDerivativeInterface >
{
  static const char* Get()
  {
    return "MetricDerivativeInterface";
  }
};
template <>
struct InterfaceName < OptimizerUpdateInterface >
{
  static const char* Get()
  {
    return "OptimizerUpdateInterface";
  }
};


// partial specialization of InterfaceName
template<template<typename> class TT, typename T1>
struct InterfaceName < TT<T1> > {
  static const char* Get()
  {
    return InterfaceName<T1>::Get();
  }
};


template <typename T>
struct AcceptorInterfaceName
{
  static const char* Get()
  {
    return InterfaceName<T>::Get();
  }
};

} // end namespace elx


#endif // #define InterfaceTraits_h