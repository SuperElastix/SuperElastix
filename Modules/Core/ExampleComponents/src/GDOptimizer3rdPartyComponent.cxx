#include "GDOptimizer3rdPartyComponent.h"

namespace selx
{
GDOptimizer3rdPartyComponent::GDOptimizer3rdPartyComponent()
{
  this->theImplementation = new Example3rdParty::GDOptimizer3rdParty();
  this->MetricObject = new Metric3rdPartyWrapper();
}
GDOptimizer3rdPartyComponent::~GDOptimizer3rdPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}


int GDOptimizer3rdPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Set(MetricDerivativeInterface* component)
{
  this->MetricObject->SetMetricDerivativeComponent(component);
  return 0;
}
int GDOptimizer3rdPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->Optimize(); // 3rd party specific call
}

bool
GDOptimizer3rdPartyComponent
::MeetsCriteria(const CriteriaType &criteria)
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(true);

  for (CriteriaType::const_iterator it = criteria.begin(); it != criteria.end(); ++it)
  {
    if (strcmp(it->first.c_str(), "NameOfClass") == 0)
    {
      if (strcmp(it->second.c_str(), this->GetNameOfClass()) != 0)
      {
        meetsCriteria = false;
        break;
      }
    }
    else if (strcmp(it->first.c_str(), "HasDerivative") == 0)
    {
      if (strcmp(it->second.c_str(), "True") != 0)
      {
        meetsCriteria = false;
        break;
      }
    }
    else if (strcmp(it->first.c_str(), "ComponentOutput") == 0)
    {
      if (strcmp(it->second.c_str(), "TransformUpdate") != 0)
      {
        meetsCriteria = false;
        break;
      }
    }
    else if (strcmp(it->first.c_str(), "ComponentInput") == 0)
    {
      if (strcmp(it->second.c_str(), "MetricValue") != 0)
      {
        meetsCriteria = false;
        break;
      }
    }
    else
    {
      meetsCriteria = false;
      hasUndefinedCriteria = true;
      break;
    }

  }
  return meetsCriteria;
}

} //end namespace selx