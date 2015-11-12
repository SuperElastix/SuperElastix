#include "SSDMetric4thPartyComponent.h"
namespace elx
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
} //end namespace elx