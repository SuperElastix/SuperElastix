#include "ComponentBase.h"

namespace selx
{


 bool ComponentBase::MeetsCriteria(const CriteriaType &criteria)
  {
    bool hasUndefinedCriteria(false);
    bool meetsCriteria(true);
    for (CriteriaType::const_iterator it = criteria.begin(); it != criteria.cend(); ++it)
    {
      if (it->first == "NameOfClass")
      //if (strcmp(it->first.c_str(), "NameOfClass") == 0)
      {
        if (it->second.size() != 1)
        {
          itkExceptionMacro("The criterion NameOfClass may have only 1 value");
        }

        //if (strcmp(it->second->c_str(), this->GetNameOfClass()) != 0)
        if (it->second[0] != this->GetNameOfClass())
        //if (strcmp((it->second.cbegin)->c_str(), this->GetNameOfClass()) != 0 )
        {
          meetsCriteria = false;
          //break;
          return false; //if there is any failed criterion, return false (like a short-circuit AND)
        }

      }

      else if (it->first == "HasAcceptingInterface")
      {
        ParameterValueType::const_iterator valueIt;
        const ParameterValueType::const_iterator valueItEnd = it->second.cend();

        for (valueIt = it->second.cbegin(); valueIt != valueItEnd; ++valueIt)
        {
          if (this->HasAcceptingInterface(valueIt->c_str()) == false)
          {
            meetsCriteria = false;
            //break;
            return false; //if there is any failed criterion, return false (like a short-circuit AND)
          }
        }

      }
      else if (it->first =="HasProvidingInterface")
      {
        ParameterValueType::const_iterator valueIt;
        const ParameterValueType::const_iterator valueItEnd = it->second.cend();

        for (valueIt = it->second.cbegin(); valueIt != valueItEnd; ++valueIt){
          if (this->HasProvidingInterface(valueIt->c_str()) == false)
          {
            meetsCriteria = false;
            //break;
            return false; //if there is any failed criterion, return false (like a short-circuit AND)
          }
        }
      }
      else if (this->MeetsCriterion(CriterionType(it->first, it->second)) == false)
      {
        meetsCriteria = false;
        return false; //if there is any failed criterion, return false (like a short-circuit AND)
      }
      else
      {
        hasUndefinedCriteria = true; //TODO 
        break;
      }

    }
    return meetsCriteria;
  }

} // end namespace selx

