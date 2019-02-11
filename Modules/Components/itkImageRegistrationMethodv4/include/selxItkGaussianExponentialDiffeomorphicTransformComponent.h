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

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkGaussianExponentialDiffeomorphicTransform.h"

namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
class ItkGaussianExponentialDiffeomorphicTransformComponent :
  public SuperElastixComponent<
  Accepting< itkImageDomainFixedInterface< Dimensionality >>,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
             UpdateInterface>>
{
public:

  /** Standard ITK typedefs. */
  typedef ItkGaussianExponentialDiffeomorphicTransformComponent<
    InternalComputationValueType, Dimensionality
    >                                       Self;
  typedef SuperElastixComponent<
    Accepting< itkImageDomainFixedInterface< Dimensionality >>,
    Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
               UpdateInterface>> Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkGaussianExponentialDiffeomorphicTransformComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkGaussianExponentialDiffeomorphicTransformComponent();

  /** Get types from interfaces */
  using TransformType      = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer   = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;
  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;

  using GaussianExponentialDiffeomorphicTransformType
      = typename itk::GaussianExponentialDiffeomorphicTransform< InternalComputationValueType, Dimensionality >;

  //Accepting Interfaces:
  virtual int Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkGaussianExponentialDiffeomorphicTransform"; } ;
  static const char * GetDescription() { return "ItkGaussianExponentialDiffeomorphicTransform Component"; }

  // Get deformation field output region after all accepters and getters have been called
  void BeforeUpdate() override;

private:

  typename GaussianExponentialDiffeomorphicTransformType::Pointer m_Transform;
  typename ItkImageDomainType::Pointer m_FixedImageDomain;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkGaussianExponentialDiffeomorphicTransformComponent" }, { keys::PixelType, PodString< InternalComputationValueType >::Get() }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkGaussianExponentialDiffeomorphicTransformComponent.hxx"
#endif
#endif // #define selxItkGaussianExponentialDiffeomorphicTransformComponent_h
