#ifndef Metric3rdPartyWrapper_h
#define Metric3rdPartyWrapper_h

#include "Example3rdPartyCode.h"
#include "Interfaces.hxx"
namespace elx
{
// An Optimizer3rdParty expects that Metric3rdParty will be set as input. All accepted interfaces by the Optimizer3rdPartyCompoment will be delegated to the Metric3rdPartyWrapper object.
  class Metric3rdPartyWrapper : public Example3rdParty::Metric3rdPartyBase  {
public:
  void SetMetricValueComponent(MetricValueInterface*);
  void SetMetricDerivativeComponent(MetricDerivativeInterface*);
  virtual int GetValue();
  virtual int GetDerivative();
private:
  MetricValueInterface* metricval;
  MetricDerivativeInterface* metricderiv;
};
} // end namespace elx
#endif // #define Metric3rdPartyWrapper_h