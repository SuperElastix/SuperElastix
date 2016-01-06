#include "SSDMetric4thPartyComponent.h"
namespace selx
{

SSDMetric4thPartyComponent::SSDMetric4thPartyComponent()
{
  this->theImplementation = new Example4thParty::SSDMetric4thParty();
}
SSDMetric4thPartyComponent::~SSDMetric4thPartyComponent()
{
  delete this->theImplementation;
}

int SSDMetric4thPartyComponent::GetValue()
{
  return this->theImplementation->GetCost(); // translate method name
};

bool
SSDMetric4thPartyComponent
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