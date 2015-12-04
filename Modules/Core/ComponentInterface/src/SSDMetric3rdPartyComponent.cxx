#include "SSDMetric3rdPartyComponent.h"
namespace elx
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
} //end namespace elx