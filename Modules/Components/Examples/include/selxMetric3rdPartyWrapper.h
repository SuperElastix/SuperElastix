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

#ifndef Metric3rdPartyWrapper_h
#define Metric3rdPartyWrapper_h

#include "selxExample3rdPartyCode.h"
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"
#include "selxExamplesInterfaces.h"
namespace selx
{
// An Optimizer3rdParty expects that Metric3rdParty will be set as input. All accepted interfaces by the Optimizer3rdPartyCompoment will be delegated to the Metric3rdPartyWrapper object.
class Metric3rdPartyWrapper : public Example3rdParty::Metric3rdPartyBase
{
public:

  void SetMetricValueComponent( MetricValueInterface::Pointer );

  void SetMetricDerivativeComponent( MetricDerivativeInterface::Pointer );

  virtual int GetValue();

  virtual int GetDerivative();

private:

  MetricValueInterface::Pointer      metricval;
  MetricDerivativeInterface::Pointer metricderiv;
};
} // end namespace selx
#endif // #define Metric3rdPartyWrapper_h
