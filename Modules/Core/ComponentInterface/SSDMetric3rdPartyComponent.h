#ifndef SSDMetric3rdPartyComponent_h
#define SSDMetric3rdPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example3rdPartyCode.h"

namespace elx
{
  // SSDMetric3rdPartyComponent provides a value and a derivative
  class SSDMetric3rdPartyComponent : public ComponentBase, public MetricDerivativeInterface, public MetricValueInterface {
  public:
    SSDMetric3rdPartyComponent();
    ~SSDMetric3rdPartyComponent();
    Example3rdParty::SSDMetric3rdParty* theImplementation;
    int GetValue();
    int GetDerivative();
  };
} //end namespace elx
#endif // #define SSDMetric3rdPartyComponent_h