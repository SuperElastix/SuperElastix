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

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include <itkTransformToDisplacementFieldFilter.h>
#include <string.h>
#include "selxMacro.h"

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

  selxNewMacro( ItkResampleFilterComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkResampleFilterComponent();
  virtual ~ItkResampleFilterComponent();

  typedef TPixel PixelType;

  // Get the type definitions from the interfaces
  typedef typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType FixedImageDomainType;
  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType    MovingImageType;
  typedef typename itkImageInterface< Dimensionality, TPixel >::ItkImageType          ResultImageType;

  typedef itk::ResampleImageFilter< MovingImageType, ResultImageType > ResampleFilterType;

  //Accepting Interfaces:
  virtual int Set( itkImageDomainFixedInterface< Dimensionality > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkTransformInterface< TInternalComputationValue, Dimensionality > * ) override;

  //Providing Interfaces:
  virtual typename ResultImageType::Pointer GetItkImage() override;

  virtual void ReconnectTransform() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkResampleFilter"; } ;
  static const char * GetDescription() { return "ItkResampleFilter Component"; }

private:

  typename ResampleFilterType::Pointer m_ResampleFilter;
  itkTransformInterface< TInternalComputationValue, Dimensionality > * m_TransformComponent;

protected:

  /* The following struct returns the string name of computation type */
  /* default implementation */

  static inline const std::string GetTypeNameString()
  {
    itkGenericExceptionMacro( << "Unknown ScalarType" << typeid( TPixel ).name() );
    // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
    // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
  }


  static inline const std::string GetPixelTypeNameString()
  {
    itkGenericExceptionMacro( << "Unknown PixelType" << typeid( TPixel ).name() );
    // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
    // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
  }
};

// unfortunately partial specialization of member functions is not allowed, without partially specializing the entire class.

/*
template <int Dimensionality>
class ItkResampleFilterComponent < Dimensionality, double >
{
  static inline const std::string GetPixelTypeNameString();
};

template <int Dimensionality>
inline const std::string
  ItkResampleFilterComponent<Dimensionality, double>
  ::GetPixelTypeNameString()
{
  return std::string("double");
}
*/

template< >
inline const std::string
ItkResampleFilterComponent< 2, float, double >
::GetPixelTypeNameString()
{
  return std::string( "float" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 2, double, double >
::GetPixelTypeNameString()
{
  return std::string( "double" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 3, float, double >
::GetPixelTypeNameString()
{
  return std::string( "float" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 3, double, double >
::GetPixelTypeNameString()
{
  return std::string( "double" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 2, float, double >
::GetTypeNameString()
{
  return std::string( "2_float" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 2, double, double >
::GetTypeNameString()
{
  return std::string( "2_double" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 3, float, double >
::GetTypeNameString()
{
  return std::string( "3_float" );
}


template< >
inline const std::string
ItkResampleFilterComponent< 3, double, double >
::GetTypeNameString()
{
  return std::string( "3_double" );
}
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkResampleFilter.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
