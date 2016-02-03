#include "selxItkImageSourceMoving.h"

namespace selx
{

  ItkImageSourceMovingComponent::ItkImageSourceMovingComponent()
{
  this->m_Source = nullptr;
}

  ItkImageSourceMovingComponent::~ItkImageSourceMovingComponent()
  {
  }

  ItkImageSourceMovingComponent::ItkImageSourceMovingType::Pointer ItkImageSourceMovingComponent::GetItkImageSourceMoving()
{
  if (this->m_Source == nullptr)
  {
    itkExceptionMacro("SourceComponent needs to be initialized by ConnectToOverlordSource()");
  }
  return this->m_Source;
}

  bool ItkImageSourceMovingComponent::ConnectToOverlordSource(itk::Object::Pointer object)
{
  this->m_Source = dynamic_cast<ItkImageSourceMovingType*>(object.GetPointer());
  return (this->m_Source == nullptr);
}


bool ItkImageSourceMovingComponent::MeetsCriterion(const CriterionType &criterion)
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
