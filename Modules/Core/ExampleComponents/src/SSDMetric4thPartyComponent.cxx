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