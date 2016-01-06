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
::MeetsCriterion(const CriterionType &criterion)
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(false);
  if (criterion.first == "ComponentProperty")
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterion.second) // auto&& preferred?
    {
      if (criterionValue != "SomeProperty")  // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = false;
      }
    }
  }
  return meetsCriteria;
}

} //end namespace selx