#include "SSDMetric3rdPartyComponent.h"
namespace selx
{
SSDMetric3rdPartyComponent::SSDMetric3rdPartyComponent()
{
  this->theImplementation = new Example3rdParty::SSDMetric3rdParty();
}
SSDMetric3rdPartyComponent::~SSDMetric3rdPartyComponent()
{
  delete this->theImplementation; 
}

int SSDMetric3rdPartyComponent::GetDerivative()
{
  return this->theImplementation->GetDerivative();
};

int SSDMetric3rdPartyComponent::GetValue()
{
  return this->theImplementation->GetValue();
};

bool
SSDMetric3rdPartyComponent
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