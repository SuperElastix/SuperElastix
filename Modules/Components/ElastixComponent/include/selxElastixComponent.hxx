#include "selxElastixComponent.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ElastixComponent< Dimensionality, TPixel>::ElastixComponent()
  {
    m_theItkFilter = TheItkFilterType::New();

    //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
    // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
  }

  template<int Dimensionality, class TPixel>
  ElastixComponent< Dimensionality, TPixel>::~ElastixComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int ElastixComponent< Dimensionality, TPixel>::Set(itkImageSourceFixedInterface<Dimensionality, TPixel>* component)
  {
    auto other = component->GetItkImageSourceFixed();
    // connect the itk pipeline
    this->m_theItkFilter->SetFixedImage(other->GetOutput());
    return 1;
  }

  template<int Dimensionality, class TPixel>
  int ElastixComponent< Dimensionality, TPixel>::Set(itkImageSourceMovingInterface<Dimensionality, TPixel>* component)
  {
    auto other = component->GetItkImageSourceMoving();
    // connect the itk pipeline
    this->m_theItkFilter->SetMovingImage(other->GetOutput());
    return 1;
  }


  template<int Dimensionality, class TPixel>
  typename ElastixComponent< Dimensionality, TPixel>::ItkImageSourcePointer ElastixComponent< Dimensionality, TPixel>::GetItkImageSource()
  {

    return (ItkImageSourcePointer) this->m_theItkFilter;
  }


  template<int Dimensionality, class TPixel>
  void ElastixComponent< Dimensionality, TPixel>::RunRegistration(void)
  {
    this->m_theItkFilter->Update();
  }


  template<int Dimensionality, class TPixel>
  bool
    ElastixComponent< Dimensionality, TPixel>
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
