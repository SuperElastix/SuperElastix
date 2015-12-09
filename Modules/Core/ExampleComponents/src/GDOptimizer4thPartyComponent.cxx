#include "GDOptimizer4thPartyComponent.h"

namespace selx
{

GDOptimizer4thPartyComponent::GDOptimizer4thPartyComponent()
{
  this->theImplementation = new Example4thParty::GDOptimizer4thParty();
  this->MetricObject = new Metric4thPartyWrapper();
}

GDOptimizer4thPartyComponent::~GDOptimizer4thPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}

int GDOptimizer4thPartyComponent::Set(MetricValueInterface* component)
{
  this->MetricObject->SetMetricValueComponent(component);
  return 0;
}

int GDOptimizer4thPartyComponent::Update()
{
  this->theImplementation->SetMetric(this->MetricObject);
  return this->theImplementation->DoOptimization(); // 4th party specific call
}

bool
GDOptimizer4thPartyComponent
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