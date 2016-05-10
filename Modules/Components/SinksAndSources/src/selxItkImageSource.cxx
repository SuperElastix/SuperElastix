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

#include "selxItkImageSource.h"

namespace selx
{

  ItkImageSourceComponent::ItkImageSourceComponent()
{
  this->m_Source = nullptr;
}

  ItkImageSourceComponent::~ItkImageSourceComponent()
  {
  }

  ItkImageSourceComponent::ItkImageSourceType::Pointer ItkImageSourceComponent::GetItkImageSource()
{
  if (this->m_Source == nullptr)
  {
    itkExceptionMacro("SourceComponent needs to be initialized by ConnectToOverlordSource()");
  }
  return this->m_Source;
}

  bool ItkImageSourceComponent::ConnectToOverlordSource(itk::Object::Pointer object)
{
  this->m_Source = dynamic_cast<ItkImageSourceType*>(object.GetPointer());
  return (this->m_Source == nullptr);
}


bool ItkImageSourceComponent::MeetsCriterion(const CriterionType &criterion)
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(false);
  if (criterion.first == "ComponentProperty")
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterion.second) // auto&& preferred?
    {
      if (criterionValue != "SomeProperty")  // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = false;
      }
    }
  }
  else if (criterion.first == "Dimensionality") //Supports this?
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterion.second) // auto&& preferred?
    {
      if (std::stoi(criterionValue) != 3) //hardcoded
      {
        meetsCriteria = false;
      }
    }

  }
  else if (criterion.first == "PixelType") //Supports this?
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterion.second) // auto&& preferred?
    {
      if (criterionValue != "double") //hardcoded
      {
        meetsCriteria = false;
      }
    }

  }
  return meetsCriteria;
}

} //end namespace selx
