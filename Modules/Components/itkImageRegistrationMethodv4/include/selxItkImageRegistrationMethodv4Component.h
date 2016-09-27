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

#ifndef selxItkImageRegistrationMethodv4Component_h
#define selxItkImageRegistrationMethodv4Component_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include <itkTransformToDisplacementFieldFilter.h>
#include <string.h>
#include "selxMacro.h"

#include "itkComposeDisplacementFieldsImageFilter.h"

namespace selx
{
template< int Dimensionality, class TPixel, class InternalComputationValueType>
class ItkImageRegistrationMethodv4Component :
  public SuperElastixComponent<
  Accepting< itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >,
  itkTransformInterface< InternalComputationValueType, Dimensionality >,
  itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >,
  itkMetricv4Interface< Dimensionality, TPixel >,
  itkOptimizerv4Interface< InternalComputationValueType >
  >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
  RunRegistrationInterface
  >
  >
{
public:

  selxNewMacro( ItkImageRegistrationMethodv4Component, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkImageRegistrationMethodv4Component();
  virtual ~ItkImageRegistrationMethodv4Component();

  typedef TPixel PixelType;
  using TransformInternalComputationValueType = InternalComputationValueType;

  // Get the type definitions from the interfaces

  typedef typename itkOptimizerv4Interface< TransformInternalComputationValueType >::InternalComputationValueType
    OptimizerInternalComputationValueType;                                                                                                                             //should be from class template

  typedef typename itkImageFixedInterface< Dimensionality, TPixel >::ItkImageType                                   FixedImageType;
  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType                                  MovingImageType;
  typedef typename itkTransformInterface< TransformInternalComputationValueType, Dimensionality >::TransformType    TransformType;
  typedef typename itkTransformInterface< TransformInternalComputationValueType, Dimensionality >::TransformPointer TransformPointer;

  using TransformParametersAdaptorsContainerInterfaceType
      = itkTransformParametersAdaptorsContainerInterface< TransformInternalComputationValueType, Dimensionality >;

  typedef itk::ImageRegistrationMethodv4< FixedImageType, MovingImageType >    TheItkFilterType;
  typedef typename TheItkFilterType::ImageMetricType                           ImageMetricType;
  typedef itk::RegistrationParameterScalesFromPhysicalShift< ImageMetricType > ScalesEstimatorType;

  //Accepting Interfaces:
  virtual int Set( itkImageFixedInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkTransformInterface< TransformInternalComputationValueType, Dimensionality > * ) override;

  virtual int Set( TransformParametersAdaptorsContainerInterfaceType * ) override;

  virtual int Set( itkMetricv4Interface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkOptimizerv4Interface< OptimizerInternalComputationValueType > * ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  virtual void RunRegistration() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkImageRegistrationMethodv4"; } ;
  static const char * GetDescription() { return "ItkImageRegistrationMethodv4 Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

  // The settings SmoothingSigmas and ShrinkFactors imply NumberOfLevels, if the user
  // provides inconsistent numbers we should detect that and report about it.
  std::string                                         m_NumberOfLevelsLastSetBy;
  TransformParametersAdaptorsContainerInterfaceType * m_TransformAdaptorsContainerInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map<std::string, std::string> TemplateProperties()
  {
    return{ { keys::NameOfClass, "ItkImageRegistrationMethodv4Component" }, { keys::PixelType, PodString<TPixel>::Get() },{ keys::InternalComputationValueType, PodString<InternalComputationValueType>::Get() }, { keys::Dimensionality, std::to_string(Dimensionality) } };
  }

};

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageRegistrationMethodv4Component.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
