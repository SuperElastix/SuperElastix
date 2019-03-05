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
#include "itkRescaleIntensityImageFilter.h"
#include "itkInvertIntensityImageFilter.h"

namespace selx
{
template< int Dimensionality, class PixelType, class InternalComputationValueType >
class ItkImageRegistrationMethodv4Component :
  public SuperElastixComponent<
  Accepting<
  itkImageFixedInterface< Dimensionality, PixelType >,
  itkImageMovingInterface< Dimensionality, PixelType >,
  itkTransformInterface< InternalComputationValueType, Dimensionality >,
  itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >,
  itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >,
  itkOptimizerv4Interface< InternalComputationValueType >
  >,
  Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
  MultiStageTransformInterface< InternalComputationValueType, Dimensionality >,
  UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkImageRegistrationMethodv4Component<
    Dimensionality, PixelType, InternalComputationValueType
    >                                     Self;
  typedef SuperElastixComponent<
    Accepting<
    itkImageFixedInterface< Dimensionality, PixelType >,
    itkImageMovingInterface< Dimensionality, PixelType >,
    itkTransformInterface< InternalComputationValueType, Dimensionality >,
    itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >,
    itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >,
    itkOptimizerv4Interface< InternalComputationValueType >
    >,
    Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
    MultiStageTransformInterface< InternalComputationValueType, Dimensionality >,
    UpdateInterface
    >
    >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkImageRegistrationMethodv4Component( const std::string & name, LoggerImpl & logger );
  virtual ~ItkImageRegistrationMethodv4Component();

  // Get the type definitions from the interfaces

  using FixedImageType   = typename itkImageFixedInterface< Dimensionality, PixelType >::ItkImageType;
  using FixedImagePointer = typename FixedImageType::Pointer;
  using MovingImageType  = typename itkImageMovingInterface< Dimensionality, PixelType >::ItkImageType;
  using MovingImagePointer = typename MovingImageType::Pointer;
  using TransformType    = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
  using TransformPointer = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;

  using CompositeTransformType = typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::CompositeTransformType;
  using TransformParametersAdaptorsContainerInterfaceType
          = itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >;

  using ImageRegistrationMethodv4Type
          = itk::ImageRegistrationMethodv4< FixedImageType, MovingImageType, TransformType >;
  using ImageMetricType = typename ImageRegistrationMethodv4Type::ImageMetricType;
  using ImageRegistrationMethodv4Pointer = typename ImageRegistrationMethodv4Type::Pointer;
  using ScalesEstimatorType = itk::RegistrationParameterScalesFromPhysicalShift< ImageMetricType >;

  using FixedRescaleImageFilterType = itk::RescaleIntensityImageFilter<FixedImageType, FixedImageType>;
  using FixedRescaleImageFilterPointer = typename FixedRescaleImageFilterType::Pointer;
  using MovingRescaleImageFilterType = itk::RescaleIntensityImageFilter<MovingImageType, MovingImageType>;
  using MovingRescaleImageFilterPointer = typename MovingRescaleImageFilterType::Pointer;
  using FixedInvertIntensityImageFilterType = itk::InvertIntensityImageFilter<FixedImageType, FixedImageType>;
  using FixedInvertIntensityImageFilterPointer = typename FixedInvertIntensityImageFilterType::Pointer;
  using MovingInvertIntensityImageFilterType = itk::InvertIntensityImageFilter<MovingImageType, MovingImageType>;
  using MovingInvertIntensityImageFilterPointer = typename MovingInvertIntensityImageFilterType::Pointer;
  using FixedImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<FixedImageType>;
  using FixedImageCalculatorFilterPointer = typename FixedImageCalculatorFilterType::Pointer;
  using MovingImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<MovingImageType>;
  using MovingImageCalculatorFilterPointer = typename MovingImageCalculatorFilterType::Pointer;

  // Accepting Interfaces:
  int Accept( typename itkImageFixedInterface< Dimensionality, PixelType >::Pointer ) override;
  int Accept( typename itkImageMovingInterface< Dimensionality, PixelType >::Pointer ) override;
  int Accept( typename itkTransformInterface< InternalComputationValueType, Dimensionality >::Pointer ) override;
  int Accept( typename TransformParametersAdaptorsContainerInterfaceType::Pointer ) override;
  int Accept( typename itkMetricv4Interface< Dimensionality, PixelType, InternalComputationValueType >::Pointer ) override;
  int Accept( typename itkOptimizerv4Interface< InternalComputationValueType >::Pointer ) override;

  // Providing Interfaces:
  TransformPointer GetItkTransform() override;
  void Update() override;
  void BeforeUpdate() override;
  void SetFixedInitialTransform( typename CompositeTransformType::Pointer fixedInitialTransform ) override;
  void SetMovingInitialTransform( typename CompositeTransformType::Pointer movingInitialTransform ) override;

  //BaseClass methods
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  bool ConnectionsSatisfied() override;

  //static const char * GetName() { return "ItkImageRegistrationMethodv4"; } ;
  static const char * GetDescription() { return "ItkImageRegistrationMethodv4 Component"; }

private:

  FixedImagePointer m_FixedImage;
  MovingImagePointer m_MovingImage;
  ImageRegistrationMethodv4Pointer m_ImageRegistrationMethodv4Filter;
  TransformPointer m_Transform;

  // The settings SmoothingSigmas and ShrinkFactors imply NumberOfLevels, if the user
  // provides inconsistent numbers we should detect that and report about it.
  std::string m_NumberOfLevelsLastSetBy;
  typename TransformParametersAdaptorsContainerInterfaceType::Pointer m_TransformAdaptorsContainerInterface;

  ComponentBase::ParameterValueType m_RescaleIntensity;
  bool m_InvertIntensity;
  double m_MetricSamplingPercentage;

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
