#include "selxItkImageRegistrationMethodv4Component.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageRegistrationMethodv4Component()
{
  m_theItkFilter = TheItkFilterType::New();
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template<int Dimensionality, class TPixel>
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::~ItkImageRegistrationMethodv4Component()
{
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSource();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage(other->GetOutput());
  return 0;
}

//ItkImageSourceType::Pointer 
template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageSourcePointer ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::GetItkImageSource()
{
  
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  FixedImageType::ConstPointer fixedImage = this->m_theItkFilter->GetFixedImage();
  MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  resampler->SetTransform(this->m_theItkFilter->GetTransform());
  resampler->SetInput(movingImage);
  resampler->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  resampler->SetOutputOrigin(fixedImage->GetOrigin());
  resampler->SetOutputSpacing(fixedImage->GetSpacing());
  resampler->SetOutputDirection(fixedImage->GetDirection());
  resampler->SetDefaultPixelValue(0);

  return resampler;
}
template<int Dimensionality, class TPixel>
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
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
  else if (criterion.first == "Dimensionality") //Supports this?
  {
    meetsCriteria = true;
    for (auto const & criterionValue : criterion.second) // auto&& preferred?
    {
      if (std::stoi(criterionValue) != Self::Dimensionality)
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
