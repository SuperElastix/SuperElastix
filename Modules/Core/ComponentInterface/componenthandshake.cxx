#include <iostream>

// test case: Assume there are two (slightly) incompatible codebases (i.e. 3rd party and 4th! party), each with an optimizer object and a metric object. The example classes of 3rd and 4th party code bases cannot be changed, but the wrapping is in our control.
// goal: make elastix components of all objects and define a handshake that checks if connections can be made.

#include "SSDMetric3rdPartyComponent.h"
#include "GDOptimizer3rdPartyComponent.h"
#include "SSDMetric4thPartyComponent.h"
#include "GDOptimizer4thPartyComponent.h"



using namespace elx;
  int main() {
    {
      std::cout << InterfaceName<MetricValueInterface>::Get() << std::endl;

      std::cout << AcceptorInterfaceName<InterfaceAcceptor<MetricValueInterface> >::Get() << std::endl;

      std::cout << InterfaceName<InterfaceAcceptor<MetricValueInterface> >::Get() << std::endl;
      std::cout << InterfaceName<InterfaceProvider<MetricValueInterface> >::Get() << std::endl;

      
    }
    {
      /************ testing interface casts ***********
      * expected: ok
      */
      SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
      ComponentBase* metric3p = tempmetric3p;

      MetricValueInterface* valIF = dynamic_cast<MetricValueInterface*> (metric3p);
      std::cout << valIF->GetValue() << std::endl;

      MetricDerivativeInterface* derIF = dynamic_cast<MetricDerivativeInterface*> (metric3p);
      std::cout << derIF->GetDerivative() << std::endl;
    }
    /************ Connect metric4p to optimizer4p ***********
    * expected: ok
    */
  {
    SSDMetric4thPartyComponent* tempmetric4p = new SSDMetric4thPartyComponent();
    ComponentBase* metric4p = tempmetric4p; // type returned by our component factory

    GDOptimizer4thPartyComponent* tempOptimizer4p = new GDOptimizer4thPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p; // type returned by our component factory

    interfaceStatus IFstatus = optimizer4p->ConnectFrom("MetricValueInterface", metric4p);

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    opValIF->Connect(metric4p);

    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    opUpdIF->Update();
  }
  /************ Connect metric3p to optimizer4p ***********
  * expected: ok
  * optimizer4p will only use/have access to the GetValue interface of metric3p
  */
  {
    SSDMetric3rdPartyComponent* tempmetric3p = new SSDMetric3rdPartyComponent();
    ComponentBase* metric3p = tempmetric3p; // type returned by our component factory

    GDOptimizer4thPartyComponent* tempOptimizer4p = new GDOptimizer4thPartyComponent();
    ComponentBase* optimizer4p = tempOptimizer4p; // type returned by our component factory

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    if (!opValIF->Connect(metric3p))
    {
      std::cout << "metric3p cannot connect to optimizer4p by ValueInterface" << std::endl;
    }

    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer4p);
    if (!opValIF)
    {
      std::cout << "optimizer4p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    opUpdIF->Update();
  }
  /************ Connect metric4p to optimizer3p ***********
  * expected: fail
  * optimizer3p needs a metric with GetDerivative which metric4p doesn't have
  */
  {
    SSDMetric4thPartyComponent* tempmetric4p = new SSDMetric4thPartyComponent();
    ComponentBase* metric4p = tempmetric4p; // type returned by our component factory

    GDOptimizer3rdPartyComponent* tempOptimizer3p = new GDOptimizer3rdPartyComponent();
    ComponentBase* optimizer3p = tempOptimizer3p; // type returned by our component factory

    InterfaceAcceptor<MetricValueInterface>* opValIF = dynamic_cast<InterfaceAcceptor<MetricValueInterface>*> (optimizer3p);
    if (!opValIF)
    {
      std::cout << "optimizer3p has no OptimizerValueInterface" << std::endl;
    }

    // connect value interfaces
    if (!opValIF->Connect(metric4p))
    {
      std::cout << "metric4p cannot connect to optimizer3p by ValueInterface" << std::endl;
    }

    //opValIF->Set(tempmetric4p);

    InterfaceAcceptor<MetricDerivativeInterface>* opDerivIF = dynamic_cast<InterfaceAcceptor<MetricDerivativeInterface>*> (optimizer3p);
    if (!opDerivIF)
    {
      std::cout << "optimizer3p has no OptimizerDerivativeInterface" << std::endl;
    }
    // connect derivative interfaces
    if (!opDerivIF->Connect(metric4p))
    {
      std::cout << "metric4p cannot connect to optimizer3p by DerivativeInterface" << std::endl;
    }


    OptimizerUpdateInterface* opUpdIF = dynamic_cast<OptimizerUpdateInterface*> (optimizer3p);
    if (!opValIF)
    {
      std::cout << "optimizer3p has no OptimizerUpdateInterface" << std::endl;
    }

    // Update the optimizer component 
    // opUpdIF->Update(); // will fail since the metric does'nt have GetDerivative()
  }
  return 0;
  }
