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

#ifndef selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent_h
#define selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkImageRegistrationMethodv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include "itkTransformToDisplacementFieldFilter.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"
#include "itkArray.h"

namespace selx
{
template< int Dimensionality, class TransformInternalComputationValueType >
class ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent :
  public SuperElastixComponent<
  Accepting< itkImageDomainFixedInterface< Dimensionality >>,
  Providing< itkTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType, Dimensionality >, UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent<
    Dimensionality, TransformInternalComputationValueType
    >                                       Self;
  typedef SuperElastixComponent<
    Accepting< itkImageDomainFixedInterface< Dimensionality >>,
    Providing< itkTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType, Dimensionality >, UpdateInterface
    >
    >                                       Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent();

  // Get the type definitions from the interfaces
  typedef typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType FixedImageDomainType;

  using itkTransformParametersAdaptorsContainerInterfaceType
      = itkTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType, Dimensionality >;
  using TransformParametersAdaptorsContainerType
      = typename itkTransformParametersAdaptorsContainerInterfaceType::TransformParametersAdaptorsContainerType;
  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;
  using ItkImageDomainPointer = typename ItkImageDomainType::Pointer;

  // Since the itkTransformParametersAdaptorsContainerInterface is only defined by BaseType Adaptors and Transforms, we cannot use the ItkTransformParametersAdaptorsContainerInterfaceType::TransformParametersAdaptorBaseType;
  // Specific to this componenent is the full definition of TransformParametersAdaptorType being GaussianExponentialDiffeomorphic
  using TransformParametersAdaptorType
      = itk::GaussianExponentialDiffeomorphicTransformParametersAdaptor< itk::GaussianExponentialDiffeomorphicTransform<
    TransformInternalComputationValueType, Dimensionality >>;

  using ShrinkFactorsArrayType = itk::Array< itk::SizeValueType >;
  using SmoothingSigmasArrayType = itk::Array< TransformInternalComputationValueType >;

  //Accepting Interfaces:
  int Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer ) override;

  //Providing Interfaces:
  TransformParametersAdaptorsContainerType GetItkTransformParametersAdaptorsContainer() override;

  void BeforeUpdate() override;

  //BaseClass methods
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainer Component"; }

private:

  TransformParametersAdaptorsContainerType m_Adaptors;

  // Shrink the virtual domain by specified factors for each level.  See documentation
  // for the itkShrinkImageFilter for more detailed behavior.
  ShrinkFactorsArrayType m_ShrinkFactorsPerLevel;
  // Smooth by specified gaussian sigmas for each level.  These values are specified in
  // physical units.

  SmoothingSigmasArrayType m_SmoothingSigmasPerLevel;

  ItkImageDomainPointer m_FixedDomain;

protected:

  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" }, { keys::PixelType, PodString< TransformInternalComputationValueType >::Get() }, { keys::InternalComputationValueType, PodString< TransformInternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
