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
::MeetsCriterium(const CriteriumType &criterium)
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(false);
  if (strcmp(criterium.first.c_str(), "ComponentProperty") == 0)
  {
    if (strcmp(criterium.second.c_str(), "SomeProperty") == 0) // e.g. "GradientDescent", "SupportsSparseSamples
    {
      meetsCriteria = true;
    }
  }
  return meetsCriteria;
}

} //end namespace selx