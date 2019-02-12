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

#ifndef selxItkTransformDisplacementFilterComponent_h
#define selxItkTransformDisplacementFilterComponent_h

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

namespace selx
{
template< int Dimensionality, class TPixel, class TInternalComputationValue >
class ItkTransformDisplacementFilterComponent :
  public SuperElastixComponent<
  Accepting< itkTransformInterface< TInternalComputationValue, Dimensionality >,
  itkImageDomainFixedInterface< Dimensionality >
  >,
  Providing< itkDisplacementFieldInterface< Dimensionality, TPixel >,
  UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkTransformDisplacementFilterComponent<
    Dimensionality, TPixel, TInternalComputationValue
    >                                     Self;
  typedef SuperElastixComponent<
    Accepting< itkTransformInterface< TInternalComputationValue, Dimensionality >,
    itkImageDomainFixedInterface< Dimensionality >
    >,
    Providing< itkDisplacementFieldInterface< Dimensionality, TPixel >,
    UpdateInterface
    >
    >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkTransformDisplacementFilterComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkTransformDisplacementFilterComponent();

  typedef TPixel PixelType;

  // Get the type definitions from the interfaces
  using itkImageDomainFixedType    = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;
  using DisplacementFieldType =  typename itkDisplacementFieldInterface< Dimensionality, TPixel >::ItkDisplacementFieldType;

  using DisplacementFieldFilterType = itk::TransformToDisplacementFieldFilter< DisplacementFieldType >;

  //Accepting Interfaces:
  int Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer ) override;

  int Accept( typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer ) override;

  //Providing Interfaces:
  typename DisplacementFieldType::Pointer GetItkDisplacementField() override;

  void BeforeUpdate() override;
  void Update() override;

  //BaseClass methods
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkTransformDisplacementFilterComponent"; } ;
  static const char * GetDescription() { return "ItkTransformDisplacementFilter Component"; }

private:

  typename DisplacementFieldFilterType::Pointer m_DisplacementFieldFilter;
  typename itk::ImageBase<Dimensionality>::Pointer m_ImageDomainFixed;
  typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer m_TransformComponent;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkTransformDisplacementFilterComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::InternalComputationValueType, PodString< TInternalComputationValue >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkTransformDisplacementFilterComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
