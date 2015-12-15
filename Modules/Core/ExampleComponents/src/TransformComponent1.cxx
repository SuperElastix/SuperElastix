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
#ifndef TransformComponent1_hxx
#define TransformComponent1_hxx

#include "TransformComponent1.h"

namespace selx
{
  TransformComponent1::TransformComponent1()
  {
  }

  bool
  TransformComponent1::MeetsCriterium(const CriteriumType &criterium)
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
    else if (strcmp(criterium.first.c_str(), "ComponentOutput") == 0)
      {
        if (strcmp(criterium.second.c_str(), "Transform") != 0)
        {
          meetsCriteria = false;
        }
      }
      else if (strcmp(criterium.first.c_str(), "ComponentInput") == 0)
      {
        if (strcmp(criterium.second.c_str(), "Sampler") != 0)
        {
          meetsCriteria = false;
        }
      }
    return meetsCriteria;
  }
  

} // end namespace selx

#endif
