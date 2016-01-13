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
  return meetsCriteria;
}

} //end namespace selx
