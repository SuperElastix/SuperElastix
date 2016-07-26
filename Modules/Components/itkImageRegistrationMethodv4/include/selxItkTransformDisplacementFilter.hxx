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

#include "selxItkTransformDisplacementFilter.h"

namespace selx
{

  template <int Dimensionality, class TPixel, class TInternalComputationValue>
  ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>::ItkTransformDisplacementFilter()
{
  m_DisplacementFieldFilter = DisplacementFieldFilterType::New();
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

  template <int Dimensionality, class TPixel, class TInternalComputationValue>
  ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>::~ItkTransformDisplacementFilter()
{
}

template <int Dimensionality, class TPixel, class TInternalComputationValue>
int ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>
::Set(itkImageFixedInterface<Dimensionality, TPixel>* component)
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  //this->m_DisplacementFieldFilter->SetSize(fixedImage->GetBufferedRegion().GetSize()); //should be virtual image...
  this->m_DisplacementFieldFilter->SetSize(fixedImage->GetLargestPossibleRegion().GetSize()); //should be virtual image...
  this->m_DisplacementFieldFilter->SetOutputOrigin(fixedImage->GetOrigin());
  this->m_DisplacementFieldFilter->SetOutputSpacing(fixedImage->GetSpacing());
  this->m_DisplacementFieldFilter->SetOutputDirection(fixedImage->GetDirection());
  this->m_DisplacementFieldFilter->UpdateOutputInformation();

   return 0;
}

template <int Dimensionality, class TPixel, class TInternalComputationValue>
int ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>
::Set(itkTransformInterface<TInternalComputationValue, Dimensionality >* component)
{
  auto transform = component->GetItkTransform();
  // connect the itk pipeline
  this->m_DisplacementFieldFilter->SetTransform(transform.GetPointer());

  return 0;
}


template <int Dimensionality, class TPixel, class TInternalComputationValue>
typename ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>::DisplacementFieldImageType::Pointer
ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>
::GetDisplacementFieldItkImage()
{

  return this->m_DisplacementFieldFilter->GetOutput();
}


template <int Dimensionality, class TPixel, class TInternalComputationValue>
bool
ItkTransformDisplacementFilter< Dimensionality, TPixel, TInternalComputationValue>
::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
      if (std::stoi(criterionValue) != Dimensionality)
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
      if (criterionValue != Self::GetPixelTypeNameString())
      {
        meetsCriteria = false;
      }
    }

  }
  return meetsCriteria;
}

} //end namespace selx
