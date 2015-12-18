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
  if (criterium.first == "ComponentProperty")
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterium.second) // auto&& preferred?
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