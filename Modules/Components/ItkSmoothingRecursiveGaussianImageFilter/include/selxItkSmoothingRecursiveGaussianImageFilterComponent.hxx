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
int ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::Set(itkImageSourceInterface* component)
{
  auto other = component->GetItkImageSource();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput(other->GetOutput());
  return 0;
}

//ItkImageSourceType::Pointer 
template<int Dimensionality, class TPixel>
typename ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::ItkImageSourcePointer ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>::GetItkImageSource()
{
  return m_theItkFilter;
}
template<int Dimensionality, class TPixel>
bool
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel>
::MeetsCriterion(const CriterionType &criterion)
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
  return meetsCriteria;
}

} //end namespace selx
