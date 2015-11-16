#ifndef Interfaces_hxx
#define Interfaces_hxx

#include "ComponentBase.h"
#include <typeinfo>

namespace elx
{
// Define the providing interfaces abstractly
class MetricDerivativeInterface {
  public:
    virtual int GetDerivative() = 0;
};

class MetricValueInterface {
  public:
    virtual int GetValue() = 0;
};

class OptimizerUpdateInterface {
public:
  virtual int Update() = 0;
};

// Define the accepting interfaces as templated by the providing interface

template<class InterfaceT>
class InterfaceAcceptor {
public:
  virtual int Set(InterfaceT*) = 0;
  int Connect(ComponentBase*); 
private:
  bool isSet;
};


//template<typename... Interfaces>
//class Accepting : public Interfaces...
//{
//};

//template<>
//class Accepting<>
//{
//};


//template<typename FirstInterface>
//class Accepting : public InterfaceAcceptor < FirstInterface >
//{
//};

template<typename ... RestInterfaces>
class Accepting
{
};


//template<typename FirstInterface, typename ... RestInterfaces>
//class Accepting<FirstInterface, RestInterfaces... > : public InterfaceAcceptor<FirstInterface>, public Accepting< RestInterfaces ... >
//{
//};

template<typename FirstInterface, typename ... RestInterfaces>
class Accepting<FirstInterface, RestInterfaces... > : public Accepting< RestInterfaces ... >, public InterfaceAcceptor<FirstInterface>
{
  //FirstInterface firstIF;
};



template<typename... Interfaces>
class Providing : public Interfaces...
{
};

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
class Implements : public ComponentBase, public AcceptingInterfaces, public ProvidingInterfaces
{
  typedef AcceptingInterfaces AcceptingInterfacesType;
  typedef ProvidingInterfaces ProvidingInterfacesType;
};

/*
// 

template <typename First, typename ... Rest>
int ConnectFromBaseQueryInterface(GUID const & id) noexcept
{
  if (id == __uuidof(First) || id == __uuidof(::IUnknown))
  {
    return static_cast<First *>(this);
  }
  if (IsInspectable<Interfaces ...>() &&
    id == __uuidof(::IInspectable))
  {
    return FindInspectable<Interfaces ...>();
  }
  return FindInterface<Rest ...>(id);
}
*/
// TEST
template<class InterfaceT>
class InterfaceProvider {
public:
  virtual int Set(InterfaceT*) {};
private:
};


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



template<class InterfaceT>
int InterfaceAcceptor<InterfaceT>::Connect(ComponentBase* providerComponent){

    InterfaceT* providerInterface = dynamic_cast<InterfaceT*> (providerComponent);
    if (!providerInterface)
    {
      std::cout << "providerComponent does not have required " << InterfaceName < InterfaceT >::Get() << std::endl;
      return 0;
    }
    // connect value interfaces
    this->Set(providerInterface); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible
    return 1;
  }

} // end namespace elx
#endif // #define Interfaces_hxx