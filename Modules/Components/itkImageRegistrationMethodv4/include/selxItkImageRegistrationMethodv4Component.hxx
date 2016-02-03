#include "selxItkImageRegistrationMethodv4Component.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageRegistrationMethodv4Component()
{
  m_theItkFilter = TheItkFilterType::New();
  m_resampler = ResampleFilterType::New();
  
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template<int Dimensionality, class TPixel>
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::~ItkImageRegistrationMethodv4Component()
{
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceFixedInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSourceFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage(other->GetOutput());
  return 1;
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceMovingInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSourceMoving();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage(other->GetOutput());
  return 1;
}



template<int Dimensionality, class TPixel>
void ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::RunRegistration(void)
{
  this->m_theItkFilter->Update();
  FixedImageType::ConstPointer fixedImage = this->m_theItkFilter->GetFixedImage();
  MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  this->m_resampler->SetTransform(this->m_theItkFilter->GetTransform());
  this->m_resampler->SetInput(movingImage);
  this->m_resampler->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  this->m_resampler->SetOutputOrigin(fixedImage->GetOrigin());
  this->m_resampler->SetOutputSpacing(fixedImage->GetSpacing());
  this->m_resampler->SetOutputDirection(fixedImage->GetDirection());
  this->m_resampler->SetDefaultPixelValue(0);
}

template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageSourcePointer ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::GetItkImageSource()
{
  
  return (ItkImageSourcePointer) this->m_resampler;
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
