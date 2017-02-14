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

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkImageRegistrationMethodv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include "itkTransformToDisplacementFieldFilter.h"
#include "itkComposeDisplacementFieldsImageFilter.h"

namespace selx
{
template< int Dimensionality, class PixelType, class InternalComputationValueType >
class ItkImageRegistrationMethodv4Component :
  public SuperElastixComponent<
  Accepting< itkImageFixedInterface< Dimensionality, PixelType >,
  itkImageMovingInterface< Dimensionality, PixelType >,
  itkTransformInterface< InternalComputationValueType, Dimensionality >,
  itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >,
  itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >,
  itkOptimizerv4Interface< InternalComputationValueType >
  >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
  MultiStageTransformInterface< InternalComputationValueType, Dimensionality >,
  RunRegistrationInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkImageRegistrationMethodv4Component          Self;
  typedef ComponentBase                       Superclass;
  typedef std::shared_ptr< Self >           Pointer;
  typedef std::shared_ptr< const Self >     ConstPointer;

  ItkImageRegistrationMethodv4Component(const std::string & name);
  virtual ~ItkImageRegistrationMethodv4Component();

  // Get the type definitions from the interfaces

  using FixedImageType   = typename itkImageFixedInterface< Dimensionality, PixelType >::ItkImageType;
  using MovingImageType  = typename itkImageMovingInterface< Dimensionality, PixelType >::ItkImageType;
  using TransformType    = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;

  using CompositeTransformType = typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::CompositeTransformType;
  using TransformParametersAdaptorsContainerInterfaceType
      = itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >;

  typedef itk::ImageRegistrationMethodv4< FixedImageType, MovingImageType, TransformType > TheItkFilterType;
  typedef typename TheItkFilterType::ImageMetricType                                       ImageMetricType;
  typedef itk::RegistrationParameterScalesFromPhysicalShift< ImageMetricType >             ScalesEstimatorType;

  //Accepting Interfaces:
  virtual int Set(typename itkImageFixedInterface< Dimensionality, PixelType >::Pointer) override;

  virtual int Set(typename itkImageMovingInterface< Dimensionality, PixelType >::Pointer) override;

  virtual int Set(typename itkTransformInterface< InternalComputationValueType, Dimensionality >::Pointer) override;

  virtual int Set( typename TransformParametersAdaptorsContainerInterfaceType::Pointer ) override;

  virtual int Set( typename itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >::Pointer ) override;

  virtual int Set( typename itkOptimizerv4Interface< InternalComputationValueType >::Pointer ) override;

  //Providing Interfaces:
  virtual TransformPointer GetItkTransform() override;

  virtual void RunRegistration() override;

  virtual void SetFixedInitialTransform(typename CompositeTransformType::Pointer fixedInitialTransform) override;
  virtual void SetMovingInitialTransform(typename CompositeTransformType::Pointer movingInitialTransform) override;

  virtual const typename std::string GetComponentName() override;

  //BaseClass methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkImageRegistrationMethodv4"; } ;
  static const char * GetDescription() { return "ItkImageRegistrationMethodv4 Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

  // The settings SmoothingSigmas and ShrinkFactors imply NumberOfLevels, if the user
  // provides inconsistent numbers we should detect that and report about it.
  std::string                                         m_NumberOfLevelsLastSetBy;
  typename TransformParametersAdaptorsContainerInterfaceType::Pointer m_TransformAdaptorsContainerInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkImageRegistrationMethodv4Component" }, { keys::PixelType, PodString< PixelType >::Get() }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageRegistrationMethodv4Component.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
