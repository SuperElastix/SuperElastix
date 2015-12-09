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