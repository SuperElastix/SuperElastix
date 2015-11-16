#include "GDOptimizer3rdPartyComponent.h"

namespace elx
{
GDOptimizer3rdPartyComponent::GDOptimizer3rdPartyComponent()
{
  this->theImplementation = new Example3rdParty::GDOptimizer3rdParty();
  this->MetricObject = new Metric3rdPartyWrapper();
}
GDOptimizer3rdPartyComponent::~GDOptimizer3rdPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}


int GDOptimizer3rdPartyComponent::ConnectFrom(const char * interfacename, ComponentBase* other)
{
  //TODO write out as templated functionality:
  if (std::strcmp(InterfaceName<InterfaceAcceptor<MetricValueInterface>>::Get(), interfacename) == 0)
  {
    InterfaceAcceptor<MetricValueInterface>* acceptIF = static_cast<InterfaceAcceptor<MetricValueInterface>*> (this);
    if (!acceptIF)
    {
      std::cout << InterfaceName<InterfaceAcceptor<MetricValueInterface>>::Get() << " optimizer has no OptimizerValueInterface" << std::endl;
    }
    // connect value interfaces
    acceptIF->Connect(other);
    return 1; //success, assume only one interface listens interfacename
  }
  if (std::strcmp(InterfaceName<InterfaceAcceptor<MetricDerivativeInterface>>::Get(), interfacename) == 0)
  {
    InterfaceAcceptor<MetricDerivativeInterface>* acceptIF = static_cast<InterfaceAcceptor<MetricDerivativeInterface>*> (this);
    if (!acceptIF)
    {
      std::cout << InterfaceName<InterfaceAcceptor<MetricDerivativeInterface>>::Get() << " optimizer has no OptimizerValueInterface" << std::endl;
    }
    // connect value interfaces
    acceptIF->Connect(other);
    return 1; //success, assume only one interface listens interfacename
  }
  return 0;
}

int GDOptimizer3rdPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Set(MetricDerivativeInterface* component)
{
  this->MetricObject->SetMetricDerivativeComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->Optimize(); // 3rd party specific call
}
} //end namespace elx