#ifndef Metric4thPartyWrapper_h
#define Metric4thPartyWrapper_h

#include "Example4thPartyCode.h"
#include "Interfaces.hxx"
namespace selx
{
// An Optimizer4thParty expects that Metric4thParty will be set as input. All accepted interfaces by the Optimizer4thPartyCompoment will be delegated to the Metric4thPartyWrapper object.
  class Metric4thPartyWrapper : public Example4thParty::Metric4thPartyBase  {
public:
  void SetMetricValueComponent(MetricValueInterface*);
  virtual int GetCost();
private:
  MetricValueInterface* metricval;
};
} // end namespace selx
#endif // #define Metric3rdPartyWrapper_h