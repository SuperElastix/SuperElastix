/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxExample3rdPartyCode.h"
namespace Example3rdParty
{
GDOptimizer3rdParty::GDOptimizer3rdParty()
{
  this->theMetric = nullptr;
}


GDOptimizer3rdParty::~GDOptimizer3rdParty()
{
}


int
GDOptimizer3rdParty::SetMetric( Metric3rdPartyBase * metric )
{
  this->theMetric = metric;
  return 0;
}


int
GDOptimizer3rdParty::Optimize()
{
  if( this->theMetric != nullptr )
  {
    std::cout << "GDOptimizer3rdParty->Optimize():" << std::endl;
    std::cout << "  theMetric->GetValue():" << theMetric->GetValue() << std::endl;
    std::cout << "  theMetric->GetDerivative():" << theMetric->GetDerivative() << std::endl;
  }
  return 0;
}
} // end namespace Example3rdParty
