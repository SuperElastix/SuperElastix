#ifndef GDOptimizer4thPartyComponent_h
#define GDOptimizer4thPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example4thPartyCode.h"
#include "Metric4thPartyWrapper.h"

namespace elx
{
  // wrapping into components: 
  class GDOptimizer4thPartyComponent : public ComponentBase, public InterfaceAcceptor<MetricValueInterface>, public OptimizerUpdateInterface
  {
  public:
    GDOptimizer4thPartyComponent();
    ~GDOptimizer4thPartyComponent();
    Example4thParty::GDOptimizer4thParty* theImplementation;
    Metric4thPartyWrapper* MetricObject;
    virtual int ConnectFrom(const char *, ComponentBase*);
    int Set(MetricValueInterface*);
    int Update();
  };

} //end namespace elx
#endif // #define GDOptimizer4thPartyComponent_h