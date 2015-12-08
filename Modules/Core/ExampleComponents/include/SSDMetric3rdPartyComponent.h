#ifndef SSDMetric3rdPartyComponent_h
#define SSDMetric3rdPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example3rdPartyCode.h"

namespace selx
{
  // SSDMetric3rdPartyComponent provides a value and a derivative
  class SSDMetric3rdPartyComponent : 
    public Implements<
    Accepting<>,
    Providing< MetricDerivativeInterface, MetricValueInterface>
    >
  {
  public:
    SSDMetric3rdPartyComponent();
    virtual ~SSDMetric3rdPartyComponent();
    Example3rdParty::SSDMetric3rdParty* theImplementation;
    int GetValue();
    int GetDerivative();
    virtual bool MeetsCriteria(const CriteriaType &criteria) { return false; };
    static const char * GetName(){ return "SSDMetric3rdPartyComponent"; }
    static const char * GetDescription(){ return "SSD Metric 3rd Party Component"; };
  };
} //end namespace selx
#endif // #define SSDMetric3rdPartyComponent_h