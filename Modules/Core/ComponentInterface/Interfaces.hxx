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
  virtual int Set(InterfaceT*) = 0;
  int Connect(ComponentBase*); 
private:
  bool isSet;
};

template<class InterfaceT>
int InterfaceAcceptor<InterfaceT>::Connect(ComponentBase* providerComponent){

    InterfaceT* providerInterface = dynamic_cast<InterfaceT*> (providerComponent);
    if (!providerInterface)
    {
      std::cout << "providerComponent does not have required interface" << std::endl;
      return 0;
    }
    // connect value interfaces
    this->Set(providerInterface); // due to the input argument being uniquely defined in the multiple inheritance tree, all versions of Set() are accessible
    return 1;
  }

} // end namespace elx
#endif // #define Interfaces_hxx