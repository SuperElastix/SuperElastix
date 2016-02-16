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
