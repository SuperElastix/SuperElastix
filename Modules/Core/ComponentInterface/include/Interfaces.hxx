#ifndef Interfaces_hxx
#define Interfaces_hxx

#include "InterfaceTraits.h"
namespace elx
{
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

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
interfaceStatus Implements<AcceptingInterfaces, ProvidingInterfaces>::ConnectFrom(const char * interfacename, ComponentBase* other)
{
  return AcceptingInterfaces::ConnectFromImpl(interfacename, other);
}

template<typename AcceptingInterfaces, typename ProvidingInterfaces>
int Implements<AcceptingInterfaces, ProvidingInterfaces>::ConnectFrom(ComponentBase* other)
{
  return AcceptingInterfaces::ConnectFromImpl(other);
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

template<typename FirstInterface, typename ... RestInterfaces>
int Accepting<FirstInterface, RestInterfaces... >::ConnectFromImpl(ComponentBase* other)
{
  // static_cast always succeeds since we know via the template arguments of the component which InterfaceAcceptors its base classes are.
  InterfaceAcceptor<FirstInterface>* acceptIF = static_cast<InterfaceAcceptor<FirstInterface>*> (this);

  // See if the other component has the right interface and try to connect them
  // count the number of successes
  return acceptIF->Connect(other) + Accepting< RestInterfaces ... >::ConnectFromImpl(other);
}

} // end namespace elx


#endif // #define Interfaces_hxx