#include "selxItkImageSourceFixed.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageSourceFixedComponent< Dimensionality, TPixel>::ItkImageSourceFixedComponent()
  {
    this->m_Source = nullptr;
  }

  template<int Dimensionality, class TPixel>
  ItkImageSourceFixedComponent< Dimensionality, TPixel>::~ItkImageSourceFixedComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  typename ItkImageSourceFixedComponent< Dimensionality, TPixel>::ItkImageSourceFixedType::Pointer ItkImageSourceFixedComponent< Dimensionality, TPixel>::GetItkImageSourceFixed()
  {
    if (this->m_Source == nullptr)
    {
      itkExceptionMacro("SourceComponent needs to be initialized by ConnectToOverlordSource()");
    }
    return this->m_Source;
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSourceFixedComponent< Dimensionality, TPixel>::ConnectToOverlordSource(itk::Object::Pointer object)
  {
    this->m_Source = dynamic_cast<ItkImageSourceFixedType*>(object.GetPointer());
    return (this->m_Source == nullptr);
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSourceFixedComponent< Dimensionality, TPixel>::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
