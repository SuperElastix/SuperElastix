#include "Example4thPartyCode.h"
namespace Example4thParty
{
GDOptimizer4thParty::GDOptimizer4thParty()
{
  this->theMetric = nullptr;
}
GDOptimizer4thParty::~GDOptimizer4thParty()
{
}
int GDOptimizer4thParty::SetMetric(Metric4thPartyBase* metric)
{
  this->theMetric = metric;
  return 0;
}
int GDOptimizer4thParty::DoOptimization()
{
  if (this->theMetric != nullptr)
  {
    std::cout << "GDOptimizer4thParty->DoOptimization():" << std::endl;
    std::cout << "  theMetric->GetCost():" << theMetric->GetCost() << std::endl;
  }
  return 0;
}
} // end namespace Example4thParty