/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical 
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxItkGaussianExponentialDiffeomorphicTransform.h"

namespace selx
{
  template<class InternalComputationValueType, int Dimensionality>
  ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType,Dimensionality>::ItkGaussianExponentialDiffeomorphicTransformComponent()
{
  m_Transform = GaussianExponentialDiffeomorphicTransformType::New();
  

  

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template<class InternalComputationValueType, int Dimensionality>
ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>::~ItkGaussianExponentialDiffeomorphicTransformComponent()
{
}

template<class InternalComputationValueType, int Dimensionality>
int ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>
::Set(itkImageDomainFixedInterface<Dimensionality>* component)
{
  this->m_FixedImageDomain = component->GetItkImageDomainFixed();

  auto displacementField = GaussianExponentialDiffeomorphicTransformType::DisplacementFieldType::New();
  //auto zeroVector = itk::NumericTraits<GaussianExponentialDiffeomorphicTransformType::DisplacementFieldType::PixelType>::Zero();
  auto zeroVector = typename GaussianExponentialDiffeomorphicTransformType::DisplacementFieldType::PixelType(0.0);

  displacementField->CopyInformation(this->m_FixedImageDomain);
  displacementField->SetRegions(this->m_FixedImageDomain->GetBufferedRegion());
  displacementField->Allocate();
  displacementField->FillBuffer(zeroVector);

  m_Transform->SetGaussianSmoothingVarianceForTheUpdateField(3.0);
  m_Transform->SetGaussianSmoothingVarianceForTheConstantVelocityField(6.0);
  m_Transform->SetConstantVelocityField(displacementField);
  m_Transform->SetCalculateNumberOfIntegrationStepsAutomatically(true);
  m_Transform->IntegrateVelocityField();

  return 0;
}

template<class InternalComputationValueType, int Dimensionality>
typename ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>::TransformPointer ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>::GetItkTransform()
{
  
  return (TransformPointer) this->m_Transform;
}

template<class InternalComputationValueType, int Dimensionality>
void ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>::RunRegistration()
{



}


template<class InternalComputationValueType, int Dimensionality>
bool
ItkGaussianExponentialDiffeomorphicTransformComponent< InternalComputationValueType, Dimensionality>
::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
  return meetsCriteria;
}

} //end namespace selx
