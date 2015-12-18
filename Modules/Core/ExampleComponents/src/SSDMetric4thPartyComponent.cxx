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