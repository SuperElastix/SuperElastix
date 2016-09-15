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

#ifndef selxItkGaussianExponentialDiffeomorphicTransformComponent_h
#define selxItkGaussianExponentialDiffeomorphicTransformComponent_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"
#include <string.h>
#include "selxMacro.h"
namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
class ItkGaussianExponentialDiffeomorphicTransformComponent :
  public SuperElastixComponent<
  Accepting< itkImageDomainFixedInterface< Dimensionality > >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
  RunRegistrationInterface >
  >
{
public:

  selxNewMacro( ItkGaussianExponentialDiffeomorphicTransformComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkGaussianExponentialDiffeomorphicTransformComponent();
  virtual ~ItkGaussianExponentialDiffeomorphicTransformComponent();

  /** Get types from interfaces */
  using TransformType      = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer   = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;
  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;

  using GaussianExponentialDiffeomorphicTransformType
      = typename itk::GaussianExponentialDiffeomorphicTransform< InternalComputationValueType, Dimensionality >;

  //Accepting Interfaces:
  virtual int Set( itkImageDomainFixedInterface< Dimensionality > * ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  virtual void RunRegistration() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkGaussianExponentialDiffeomorphicTransform"; } ;
  static const char * GetDescription() { return "ItkGaussianExponentialDiffeomorphicTransform Component"; }

private:

  typename GaussianExponentialDiffeomorphicTransformType::Pointer m_Transform;
  typename ItkImageDomainType::Pointer m_FixedImageDomain;

protected:

  /* The following struct returns the string name of computation type */
  /* default implementation */
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkGaussianExponentialDiffeomorphicTransform.hxx"
#endif
#endif // #define selxItkGaussianExponentialDiffeomorphicTransformComponent_h
