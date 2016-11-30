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

#ifndef selxItkTransformDisplacementFilter_h
#define selxItkTransformDisplacementFilter_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
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
  Providing< DisplacementFieldItkImageSourceInterface< Dimensionality, TPixel >,
  ReconnectTransformInterface
  >
  >
{
public:

  selxNewMacro( ItkTransformDisplacementFilterComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkTransformDisplacementFilterComponent();
  virtual ~ItkTransformDisplacementFilterComponent();

  typedef TPixel PixelType;

  // Get the type definitions from the interfaces
  using itkImageDomainFixedType    = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;
  using DisplacementFieldImageType =  typename DisplacementFieldItkImageSourceInterface< Dimensionality, TPixel >::ItkImageType;

  using DisplacementFieldFilterType = itk::TransformToDisplacementFieldFilter< DisplacementFieldImageType >;

  //Accepting Interfaces:
  virtual int Set( itkImageDomainFixedInterface< Dimensionality > * ) override;

  virtual int Set( itkTransformInterface< TInternalComputationValue, Dimensionality > * ) override;

  //Providing Interfaces:
  virtual typename DisplacementFieldImageType::Pointer GetDisplacementFieldItkImage() override;

  virtual void ReconnectTransform() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkTransformDisplacementFilterComponent"; } ;
  static const char * GetDescription() { return "ItkTransformDisplacementFilter Component"; }

private:

  typename DisplacementFieldFilterType::Pointer m_DisplacementFieldFilter;
  itkTransformInterface< TInternalComputationValue, Dimensionality > * m_TransformComponent;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkTransformDisplacementFilterComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::InternalComputationValueType, PodString< TInternalComputationValue >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkTransformDisplacementFilter.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
