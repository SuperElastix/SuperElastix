#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"

namespace selx
{
ItkSmoothingRecursiveGaussianImageFilterComponent::ItkSmoothingRecursiveGaussianImageFilterComponent()
{
  m_theItkFilter = TheItkFilterType::New();
}
ItkSmoothingRecursiveGaussianImageFilterComponent::~ItkSmoothingRecursiveGaussianImageFilterComponent()
{
}


int ItkSmoothingRecursiveGaussianImageFilterComponent::Set(itkProcessObjectInterface* component)
{
  //this->MetricObject->SetMetricValueComponent(component);
  return 0;
}

itk::ProcessObject::Pointer ItkSmoothingRecursiveGaussianImageFilterComponent::GetItkProcessObject()
{
  //this->theImplementation->SetMetric(this->MetricObject);
  return m_theItkFilter; // 3rd party specific call
}

bool
ItkSmoothingRecursiveGaussianImageFilterComponent
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
