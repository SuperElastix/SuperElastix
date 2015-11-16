#include "GDOptimizer4thPartyComponent.h"

namespace elx
{

GDOptimizer4thPartyComponent::GDOptimizer4thPartyComponent()
{
  this->theImplementation = new Example4thParty::GDOptimizer4thParty();
  this->MetricObject = new Metric4thPartyWrapper();
}

GDOptimizer4thPartyComponent::~GDOptimizer4thPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}

int GDOptimizer4thPartyComponent::ConnectFrom(const char * interfacename, ComponentBase* other)
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
  return 0;
}

int GDOptimizer4thPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}

int GDOptimizer4thPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->DoOptimization(); // 4th party specific call
}
} //end namespace elx