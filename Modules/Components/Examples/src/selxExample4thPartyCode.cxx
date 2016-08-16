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

#include "selxExample4thPartyCode.h"
namespace Example4thParty
{
GDOptimizer4thParty::GDOptimizer4thParty()
{
  this->theMetric = nullptr;
}


GDOptimizer4thParty::~GDOptimizer4thParty()
{
}


int
GDOptimizer4thParty::SetMetric( Metric4thPartyBase * metric )
{
  this->theMetric = metric;
  return 0;
}


int
GDOptimizer4thParty::DoOptimization()
{
  if( this->theMetric != nullptr )
  {
    std::cout << "GDOptimizer4thParty->DoOptimization():" << std::endl;
    std::cout << "  theMetric->GetCost():" << theMetric->GetCost() << std::endl;
  }
  return 0;
}
} // end namespace Example4thParty
