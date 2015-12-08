#ifndef SSDMetric4thPartyComponent_h
#define SSDMetric4thPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example4thPartyCode.h"

namespace selx
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
    virtual bool MeetsCriteria(const CriteriaType &criteria) { return false; };
    static const char * GetName(){ return "SSDMetric4thPartyComponent"; };
    static const char * GetDescription(){ return "SSD Metric 4th Party Component"; };
    
  };
} //end namespace selx
#endif // #define SSDMetric4thPartyComponent_h