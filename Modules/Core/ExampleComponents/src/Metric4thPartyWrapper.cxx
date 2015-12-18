#include "Metric4thPartyWrapper.h"

namespace selx
{
void Metric4thPartyWrapper::SetMetricValueComponent(MetricValueInterface* metricValueComponent)
{
  this->metricval = metricValueComponent;
}

int Metric4thPartyWrapper::GetCost()
{
  return this->metricval->GetValue();
}

} // end namespace selx