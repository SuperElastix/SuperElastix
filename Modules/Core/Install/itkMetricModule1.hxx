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
#ifndef itkMetricModule1_hxx
#define itkMetricModule1_hxx

#include "itkMetricModule1.h"

namespace itk
{
  MetricModule1::MetricModule1()
{
}

std::string MetricModule1::GetModuleTypeAsString() const
{
  std::ostringstream n;

  n << GetNameOfClass();
  n << "_";
  n << this->GetModuleTypeAsString(static_cast<float *>(ITK_NULLPTR));
  //n << "_" << this->GetInputSpaceDimension() << "_" << this->GetOutputSpaceDimension();
  return n.str();
}

LightObject::Pointer
MetricModule1::InternalClone() const
{
  // Default implementation just copies the parameters from
  // this to new Metric.
  LightObject::Pointer loPtr = Superclass::InternalClone();

  Self::Pointer rval =
    dynamic_cast<Self *>(loPtr.GetPointer());
  if(rval.IsNull())
    {
    itkExceptionMacro(<< "downcast to type "
                      << this->GetNameOfClass()
                      << " failed.");
    }
  //rval->SetFixedParameters(this->GetFixedParameters());
  //rval->SetParameters(this->GetParameters());
  return loPtr;
}


  void
  MetricModule1
  ::Read()
{
  return;
}


  void
  MetricModule1
  ::Write()
{
  return;
}

  bool
  MetricModule1
  ::MeetsCriteria(const CriteriaType& criteria)
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(true);

  for (CriteriaType::const_iterator it = criteria.begin(); it != criteria.end(); ++it)
  {
    if (strcmp(it->first.c_str(), "Name") == 0)
    {
      if (strcmp(it->second.c_str(), typeid(Self).name()) != 0)
      {
        meetsCriteria = false;
        return false;
      }
    }
    else if (strcmp(it->first.c_str(), "ModuleOutput") == 0)
    {
      if (strcmp(it->second.c_str(), "Metric") != 0)
      {
        meetsCriteria = false;
        return false;
      }
    }
    else if (strcmp(it->first.c_str(), "ModuleInput") == 0)
    {
      if (strcmp(it->second.c_str(), "Transform") != 0)
      {
        meetsCriteria = false;
        return false;
      }
    }
    else
    {
      hasUndefinedCriteria = true;
    }

  }
  return meetsCriteria;
}

} // end namespace itk

#endif
