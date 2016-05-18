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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::ItkSmoothingRecursiveGaussianImageFilterComponent()
{
  m_theItkFilter = TheItkFilterType::New();
}
template<int Dimensionality, class TPixel>
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::~ItkSmoothingRecursiveGaussianImageFilterComponent()
{
}

// TODO: see if itkImageSourceInterface is the right way to connect itk filters..
/*
int ItkSmoothingRecursiveGaussianImageFilterComponent::Set(itkProcessObjectInterface* component)
{
  auto other = component->GetItkProcessObject();
  // connect the itk pipeline
  //this->m_theItkFilter->SetInput(other->GetOutputs()[0]);
  return 0;
}

itk::ProcessObject::Pointer ItkSmoothingRecursiveGaussianImageFilterComponent::GetItkProcessObject()
{
  return m_theItkFilter;
}

int ItkSmoothingRecursiveGaussianImageFilterComponent::Set(itkImageToImageFilterInterface* component)
{
  auto other = component->GetItkImageToImageFilter();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput(other->GetOutput());
  return 0;
}

itk::ImageToImageFilter<itk::Image<double, 3>, itk::Image<double, 3>>::Pointer ItkSmoothingRecursiveGaussianImageFilterComponent::GetItkImageToImageFilter()
{
  return m_theItkFilter;
}
*/
template<int Dimensionality, class TPixel>
int ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::Set(itkImageInterface<Dimensionality, TPixel>* component)
{
  auto image = component->GetItkImage();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput(image);
  return 0;
}

//ItkImageSourceType::Pointer 
template<int Dimensionality, class TPixel>
typename ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::ItkImagePointer ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::GetItkImage()
{
  return m_theItkFilter->GetOutput();
}
template<int Dimensionality, class TPixel>
bool
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>
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
  else if (criterion.first == "Sigma") //Supports this?
  {
    if (criterion.second.size() != 1)
    {
      meetsCriteria = false;
      //itkExceptionMacro("The criterion Sigma may have only 1 value");
    }
    else
    {
      auto const & criterionValue = *criterion.second.begin();
      try
      {
        this->m_theItkFilter->SetSigma(std::stod(criterionValue));
        meetsCriteria = true;
      }
      catch (itk::ExceptionObject & err)
      {
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  return meetsCriteria;
}

} //end namespace selx
