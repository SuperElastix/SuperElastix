/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef MetricComponent1_hxx
#define MetricComponent1_hxx

#include "MetricComponent1.h"

namespace selx
{
MetricComponent1::MetricComponent1()
{
}
int MetricComponent1::Set(TransformedImageInterface* providingInterface)
{
  return 0;
}

  bool
    MetricComponent1
    ::MeetsCriterium(const CriteriumType &criterium)
  {
    bool hasUndefinedCriteria(false);
    bool meetsCriteria(false);
    if (strcmp(criterium.first.c_str(), "ComponentProperty") == 0)
    {
      if (strcmp(criterium.second.c_str(), "SomeProperty") == 0) // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = true;
      }
    }
    return meetsCriteria;
  }

} // end namespace selx

#endif
