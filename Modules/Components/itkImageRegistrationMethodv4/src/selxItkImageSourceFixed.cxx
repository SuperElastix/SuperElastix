#include "selxItkImageSourceFixed.h"

namespace selx
{

  ItkImageSourceFixedComponent::ItkImageSourceFixedComponent()
{
  this->m_Source = nullptr;
}

  ItkImageSourceFixedComponent::~ItkImageSourceFixedComponent()
  {
  }

  ItkImageSourceFixedComponent::ItkImageSourceFixedType::Pointer ItkImageSourceFixedComponent::GetItkImageSourceFixed()
{
  if (this->m_Source == nullptr)
  {
    itkExceptionMacro("SourceComponent needs to be initialized by ConnectToOverlordSource()");
  }
  return this->m_Source;
}

  bool ItkImageSourceFixedComponent::ConnectToOverlordSource(itk::Object::Pointer object)
{
  this->m_Source = dynamic_cast<ItkImageSourceFixedType*>(object.GetPointer());
  return (this->m_Source == nullptr);
}


bool ItkImageSourceFixedComponent::MeetsCriterion(const CriterionType &criterion)
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
