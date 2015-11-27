#ifndef SSDMetric4thPartyComponent_h
#define SSDMetric4thPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example4thPartyCode.h"

namespace elx
{
  // SSDMetric4thPartyComponent provides only a value and not a derivative
  class SSDMetric4thPartyComponent : 
    public Implements<
    Accepting<>,
    Providing< MetricValueInterface>
    >
  {
  public:
    SSDMetric4thPartyComponent();
    virtual ~SSDMetric4thPartyComponent();
    Example4thParty::SSDMetric4thParty* theImplementation;
    int GetValue();
  };
} //end namespace elx
#endif // #define SSDMetric4thPartyComponent_h