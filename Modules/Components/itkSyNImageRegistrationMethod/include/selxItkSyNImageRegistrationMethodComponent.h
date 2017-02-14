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

#ifndef selxItkSyNImageRegistrationMethodComponent_h
#define selxItkSyNImageRegistrationMethodComponent_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkSyNImageRegistrationMethod.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include "itkTransformToDisplacementFieldFilter.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class ItkSyNImageRegistrationMethodComponent :
  public SuperElastixComponent<
  Accepting< itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >,
  itkMetricv4Interface< Dimensionality, TPixel, double >
  >,
  Providing< itkTransformInterface< double, Dimensionality >,
  RunRegistrationInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkSyNImageRegistrationMethodComponent Self;
  typedef ComponentBase                          Superclass;
  typedef itk::SmartPointer< Self >              Pointer;
  typedef itk::SmartPointer< const Self >        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkSyNImageRegistrationMethodComponent();
  virtual ~ItkSyNImageRegistrationMethodComponent();

  typedef TPixel PixelType;
  typedef double InternalComputationValueType;

  // Get the type definitions from the interfaces
  typedef typename itkImageFixedInterface< Dimensionality, TPixel >::ItkImageType                          FixedImageType;
  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType                         MovingImageType;
  typedef typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType    TransformType;
  typedef typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer TransformPointer;

  typedef typename itkTransformInterface< InternalComputationValueType,
    Dimensionality >::InternalComputationValueType TransformInternalComputationValueType;                                                                     //should be from class template

  typedef itk::SyNImageRegistrationMethod< FixedImageType, MovingImageType >   TheItkFilterType;
  typedef typename TheItkFilterType::ImageMetricType                           ImageMetricType;
  typedef itk::RegistrationParameterScalesFromPhysicalShift< ImageMetricType > ScalesEstimatorType;

  //Accepting Interfaces:
  virtual int Set( itkImageFixedInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkMetricv4Interface< Dimensionality, TPixel, double > * ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  virtual void RunRegistration() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkSyNImageRegistrationMethod"; } ;
  static const char * GetDescription() { return "ItkSyNImageRegistrationMethod Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkSyNImageRegistrationMethodComponent" }, { keys::PixelType, PodString< PixelType >::Get() },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string(
                                                                                                           Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkSyNImageRegistrationMethodComponent.hxx"
#endif
#endif // #define selxItkSyNImageRegistrationMethodComponent_h
