#include "Metric3rdPartyWrapper.h"

namespace elx
{
void Metric3rdPartyWrapper::SetMetricValueComponent(MetricValueInterface* metricValueComponent)
{
  this->metricval = metricValueComponent;
}

int Metric3rdPartyWrapper::GetValue()
{
  return this->metricval->GetValue();
}

void Metric3rdPartyWrapper::SetMetricDerivativeComponent(MetricDerivativeInterface* metricDerivativeComponent)
{
  this->metricderiv = metricDerivativeComponent;
}

int Metric3rdPartyWrapper::GetDerivative()
{
  return this->metricderiv->GetDerivative();
}
} // end namespace elx