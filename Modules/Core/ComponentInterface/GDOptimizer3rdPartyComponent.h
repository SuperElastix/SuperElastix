#ifndef GDOptimizer3rdPartyComponent_h
#define GDOptimizer3rdPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example3rdPartyCode.h"
#include "Metric3rdPartyWrapper.h"

namespace elx
{
  class GDOptimizer3rdPartyComponent : public ComponentBase, public InterfaceAcceptor<MetricValueInterface>, public InterfaceAcceptor<MetricDerivativeInterface>, public OptimizerUpdateInterface
  {
  public:
    GDOptimizer3rdPartyComponent();
    ~GDOptimizer3rdPartyComponent();
    Example3rdParty::GDOptimizer3rdParty* theImplementation;
    Metric3rdPartyWrapper* MetricObject;
    int Set(MetricValueInterface*);
    int Set(MetricDerivativeInterface*);
    int Update();
  };
} //end namespace elx
#endif // #define GDOptimizer3rdPartyComponent_h