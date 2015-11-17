#ifndef Interfaces_hxx
#define Interfaces_hxx

#include "ComponentBase.h"

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

  // Set() is called by a succesfull Connect()
  // The implementation of Set() must be provided by component developers. 
  virtual int Set(InterfaceT*) = 0; 

  // Connect tries to connect this accepting interface with all interfaces of the provider component.
  int Connect(ComponentBase*); 

private:
  bool isSet;
};




template<typename ... RestInterfaces>
class Accepting
{
  public:
    interfaceStatus ConnectFromImpl(const char *, ComponentBase*) { return interfaceStatus::noaccepter; }; //no interface called interfacename ;
};

template<typename FirstInterface, typename ... RestInterfaces>
class Accepting<FirstInterface, RestInterfaces... > : public InterfaceAcceptor<FirstInterface>, public Accepting< RestInterfaces ... >
{
public:
  interfaceStatus ConnectFromImpl(const char *, ComponentBase*);
};




template<typename... Interfaces>
class Providing : public Interfaces...
{
};

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
class Implements : public AcceptingInterfaces, public ProvidingInterfaces, public ComponentBase
{
  public:
  virtual interfaceStatus ConnectFrom(const char *, ComponentBase*);
  //typedef typename AcceptingInterfaces AcceptingInterfacesType;
  //typedef typename ProvidingInterfaces ProvidingInterfacesType;
};


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
  this->Set(providerInterface); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible at component level
  return 1;
}

//template<template<typename... RestInterfacesT> class AcceptingT, typename ProvidingT>
//interfaceStatus Implements<AcceptingT<RestInterfacesT... >, ProvidingT>::ConnectFrom(const char * interfacename, ComponentBase* other)
//{
//  :ConnectFrom(const char * interfacename, ComponentBase* other)
//}

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
interfaceStatus Implements<AcceptingInterfaces, ProvidingInterfaces>::ConnectFrom(const char * interfacename, ComponentBase* other)
{
  return AcceptingInterfaces::ConnectFromImpl(interfacename, other);
}


template<typename FirstInterface, typename ... RestInterfaces>
interfaceStatus Accepting<FirstInterface, RestInterfaces... >::ConnectFromImpl(const char * interfacename, ComponentBase* other)
{
  // does our component have an accepting interface called interfacename? 
  if (0 ==std::strcmp(InterfaceName<InterfaceAcceptor<FirstInterface>>::Get(), interfacename))
  {
    // static_cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
    InterfaceAcceptor<FirstInterface>* acceptIF = static_cast<InterfaceAcceptor<FirstInterface>*> (this);

    // See if the other component has the right interface and try to connect them
    if (1 == acceptIF->Connect(other))
    {
      //success. By terminating this function, we assume only one interface listens to interfacename and that one connection with the other component can be made by this name
      return interfaceStatus::success;
    }
    else
    {
      // interfacename was found, but other component doesn't match
      return interfaceStatus::noprovider;
    }
  }
  return Accepting< RestInterfaces ... >::ConnectFromImpl(interfacename, other);
}

} // end namespace elx
#endif // #define Interfaces_hxx