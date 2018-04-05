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

#ifndef Metric4thPartyWrapper_h
#define Metric4thPartyWrapper_h

#include "selxExample4thPartyCode.h"
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"
#include "selxExamplesInterfaces.h"
namespace selx
{
// An Optimizer4thParty expects that Metric4thParty will be set as input. All accepted interfaces by the Optimizer4thPartyCompoment will be delegated to the Metric4thPartyWrapper object.
class Metric4thPartyWrapper : public Example4thParty::Metric4thPartyBase
{
public:

  void SetMetricValueComponent( MetricValueInterface::Pointer );

  virtual int GetCost();

private:

  MetricValueInterface::Pointer metricval;
};
} // end namespace selx
#endif // #define Metric3rdPartyWrapper_h
