#include "Example3rdPartyCode.h"
namespace Example3rdParty
{
GDOptimizer3rdParty::GDOptimizer3rdParty()
{
  this->theMetric = nullptr;
}
GDOptimizer3rdParty::~GDOptimizer3rdParty()
{
}
int GDOptimizer3rdParty::SetMetric(Metric3rdPartyBase* metric)
{
  this->theMetric = metric;
  return 0;
}
int GDOptimizer3rdParty::Optimize()
{
  if (this->theMetric != nullptr)
  {
    std::cout << "GDOptimizer3rdParty->Optimize():" << std::endl;
    std::cout << "  theMetric->GetValue():" << theMetric->GetValue() << std::endl;
    std::cout << "  theMetric->GetDerivative():" << theMetric->GetDerivative() << std::endl;
  }
  return 0;
}
} // end namespace Example3rdParty