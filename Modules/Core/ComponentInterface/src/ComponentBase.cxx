#include "ComponentBase.h"

namespace selx
{


 bool ComponentBase::MeetsCriteria(const CriteriaType &criteria)
  {
    bool hasUndefinedCriteria(false);
    bool meetsCriteria(false);
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
      else if (strcmp(it->first.c_str(), "HasAcceptingInterface") == 0)
      {
        if (this->HasAcceptingInterface(it->second.c_str()) != true)
        {
          meetsCriteria = false;
          break;
        }
      }
      else if (strcmp(it->first.c_str(), "HasProvidingInterface") == 0)
      {
        if (this->HasProvidingInterface(it->second.c_str()) != true)
        {
          meetsCriteria = false;
          break;
        }
      }
      else if (this->MeetsCriterium(CriteriumType(it->first, it->second)) != true)
      {
        meetsCriteria = false;
        break;
        
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

} // end namespace selx

