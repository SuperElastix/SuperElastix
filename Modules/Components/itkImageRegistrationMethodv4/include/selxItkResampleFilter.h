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

#ifndef selxItkResampleFilter_h
#define selxItkResampleFilter_h

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
class ItkResampleFilterComponent :
  public SuperElastixComponent<
    Accepting< itkTransformInterface< TInternalComputationValue, Dimensionality >,
      itkImageDomainFixedInterface< Dimensionality >,
      itkImageMovingInterface< Dimensionality, TPixel >
    >,
    Providing< itkImageInterface< Dimensionality, TPixel >,
      ReconnectTransformInterface
    >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkResampleFilterComponent<
    Dimensionality, TPixel, TInternalComputationValue
  >                                     Self;
  typedef SuperElastixComponent<
    Accepting< itkTransformInterface< TInternalComputationValue, Dimensionality >,
      itkImageDomainFixedInterface< Dimensionality >,
      itkImageMovingInterface< Dimensionality, TPixel >
    >,
    Providing< itkImageInterface< Dimensionality, TPixel >,
      ReconnectTransformInterface
    >
  >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkResampleFilterComponent( const std::string & name );
  virtual ~ItkResampleFilterComponent();

  typedef TPixel PixelType;

  // Get the type definitions from the interfaces
  typedef typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType FixedImageDomainType;
  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType    MovingImageType;
  typedef typename itkImageInterface< Dimensionality, TPixel >::ItkImageType          ResultImageType;

  typedef itk::ResampleImageFilter< MovingImageType, ResultImageType > ResampleFilterType;

  //Accepting Interfaces:
  virtual int Set( typename itkImageDomainFixedInterface< Dimensionality >::Pointer ) override;

  virtual int Set( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual int Set( typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer ) override;

  //Providing Interfaces:
  virtual typename ResultImageType::Pointer GetItkImage() override;

  virtual void ReconnectTransform() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkResampleFilter"; } ;
  static const char * GetDescription() { return "ItkResampleFilter Component"; }

private:

  typename ResampleFilterType::Pointer m_ResampleFilter;
  typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer m_TransformComponent;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkResampleFilterComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::InternalComputationValueType, PodString< TInternalComputationValue >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkResampleFilter.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
