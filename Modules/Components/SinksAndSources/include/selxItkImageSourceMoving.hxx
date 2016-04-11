#include "selxItkImageSourceMoving.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageSourceMovingComponent< Dimensionality, TPixel>::ItkImageSourceMovingComponent()
{
  this->m_Source = nullptr;
}

  template<int Dimensionality, class TPixel>
  ItkImageSourceMovingComponent< Dimensionality, TPixel>::~ItkImageSourceMovingComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  typename ItkImageSourceMovingComponent< Dimensionality, TPixel>::ItkImageSourceMovingType::Pointer ItkImageSourceMovingComponent< Dimensionality, TPixel>::GetItkImageSourceMoving()
{
  if (this->m_Source == nullptr)
  {
    itkExceptionMacro("SourceComponent needs to be initialized by ConnectToOverlordSource()");
  }
  return this->m_Source;
}

  template<int Dimensionality, class TPixel>
  bool ItkImageSourceMovingComponent< Dimensionality, TPixel>::ConnectToOverlordSource(itk::Object::Pointer object)
{
  this->m_Source = dynamic_cast<ItkImageSourceMovingType*>(object.GetPointer());
  return (this->m_Source == nullptr);
}

  template<int Dimensionality, class TPixel>
  bool ItkImageSourceMovingComponent< Dimensionality, TPixel>::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
