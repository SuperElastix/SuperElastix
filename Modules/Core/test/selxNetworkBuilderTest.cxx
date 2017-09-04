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

#include "selxNetworkBuilder.h"
#include "selxLogger.h"

//#include "ComponentFactory.h"
#include "selxTransformComponent1.h"
#include "selxMetricComponent1.h"
#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

#include "selxDefaultComponents.h"

#include "gtest/gtest.h"

namespace selx
{
class NetworkBuilderTest : public ::testing::Test
{
public:

  typedef std::unique_ptr< NetworkBuilderBase > NetworkBuilderPointer;
  typedef std::unique_ptr< Blueprint >          BlueprintPointer;
  typedef Blueprint::ParameterMapType           ParameterMapType;
  typedef Blueprint::ParameterValueType         ParameterValueType;

  /** register all example components */
  using CustomComponentList
      = TypeList< TransformComponent1, MetricComponent1, GDOptimizer3rdPartyComponent, GDOptimizer4thPartyComponent, SSDMetric3rdPartyComponent,
    SSDMetric4thPartyComponent >;

  virtual void SetUp()
  {
    /** make example blueprint configuration */
    blueprint = BlueprintPointer( new Blueprint() );
    ParameterMapType metricComponentParameters;
    metricComponentParameters[ "NameOfClass" ] = { "MetricComponent1" };

    ParameterMapType transformComponentParameters;
    transformComponentParameters[ "NameOfClass" ] = { "TransformComponent1" };

    blueprint->SetComponent( "Metric", metricComponentParameters );
    blueprint->SetComponent( "Transform", transformComponentParameters );

    ParameterMapType metric2TransformConnectionParameters;
    metric2TransformConnectionParameters[ "NameOfInterface" ] = { "TransformedImageInterface" };

    blueprint->SetConnection( "Transform", "Metric", metric2TransformConnectionParameters );
  }


  virtual void TearDown()
  {
  }


  BlueprintPointer blueprint;
  Logger * logger = new Logger();
};

TEST_F( NetworkBuilderTest, Create )
{
  NetworkBuilderPointer networkBuilderA = NetworkBuilderPointer( new NetworkBuilder< CustomComponentList >( *logger ) );
}

TEST_F( NetworkBuilderTest, Configure )
{
  NetworkBuilderPointer networkBuilder = NetworkBuilderPointer( new NetworkBuilder< CustomComponentList >( *logger ) );
  networkBuilder->AddBlueprint( blueprint );

  bool allUniqueComponents;
  EXPECT_NO_THROW( allUniqueComponents = networkBuilder->Configure() );
  EXPECT_TRUE( allUniqueComponents );
}

TEST_F( NetworkBuilderTest, Connect )
{
  NetworkBuilderPointer networkBuilder = NetworkBuilderPointer( new NetworkBuilder< CustomComponentList >( *logger ) );
  networkBuilder->AddBlueprint( blueprint );
  EXPECT_NO_THROW( bool allUniqueComponents = networkBuilder->Configure() );
  bool success;
  EXPECT_NO_THROW( success = networkBuilder->ConnectComponents() );
}
TEST_F( NetworkBuilderTest, DeduceComponentsFromConnections )
{
  // Fill the component database with all combinations of Dimensionality:[2,3], PixelType:[float,double] and InternalComputationValueType:[float,double]
  using RegisterComponents = TypeList<
    DisplacementFieldItkImageFilterSinkComponent< 2, double >,
    DisplacementFieldItkImageFilterSinkComponent< 2, float >,
    DisplacementFieldItkImageFilterSinkComponent< 3, double >,
    DisplacementFieldItkImageFilterSinkComponent< 3, float >,
    ItkImageSinkComponent< 2, double >,
    ItkImageSinkComponent< 2, float >,
    ItkImageSinkComponent< 3, double >,
    ItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 2, float >,
    ItkImageSourceComponent< 2, double >,
    ItkImageSourceComponent< 3, float >,
    ItkImageSourceComponent< 3, double >,
    ItkImageRegistrationMethodv4Component< 2, double, double >,
    ItkImageRegistrationMethodv4Component< 2, float, double >,
    ItkImageRegistrationMethodv4Component< 2, float, float >,
    ItkImageRegistrationMethodv4Component< 3, double, double >,
    ItkImageRegistrationMethodv4Component< 3, float, double >,
    ItkImageRegistrationMethodv4Component< 3, float, float >,
    ItkGradientDescentOptimizerv4Component< double >,
    ItkGradientDescentOptimizerv4Component< float >,
    RegistrationControllerComponent< >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, double, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float, double >,
    //ItkMeanSquaresImageToImageMetricv4Component< 2, float, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, double, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, float, double >,
    //ItkMeanSquaresImageToImageMetricv4Component< 3, float, float >,
    ItkAffineTransformComponent< double, 2 >,
    ItkAffineTransformComponent< float, 2 >,
    ItkAffineTransformComponent< double, 3 >,
    ItkAffineTransformComponent< float, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< float, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< float, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, float >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 3, double >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 3, float >,
    ItkTransformDisplacementFilterComponent< 2, double, double >,
    ItkTransformDisplacementFilterComponent< 2, float, double >,
    // ItkTransformDisplacementFilterComponent< 2, float, float >,
    ItkTransformDisplacementFilterComponent< 3, double, double >,
    ItkTransformDisplacementFilterComponent< 3, float, double >,
    //ItkTransformDisplacementFilterComponent< 3, float, float >,
    ItkResampleFilterComponent< 2, double, double >,
    ItkResampleFilterComponent< 2, float, double >,
    //ItkResampleFilterComponent< 2, float, float >,
    ItkResampleFilterComponent< 3, double, double >,
    ItkResampleFilterComponent< 3, float, double >
    // ItkResampleFilterComponent< 3, float, float >
    >;

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() ); // override old blueprint

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
                                                   { "NumberOfLevels", { "2" } },
                                                   { "ShrinkFactorsPerLevel", { "2", "1" } } } );

  blueprint->SetComponent( "FixedImageSource", { { "NameOfClass", { "ItkImageSourceComponent" } },
                                                 { "Dimensionality", { "3" } },
                                                 { "PixelType", { "float" } } } );

  blueprint->SetComponent( "MovingImageSource", { { "NameOfClass", { "ItkImageSourceComponent" } },
                                                  { "Dimensionality", { "3" } },
                                                  { "PixelType", { "float" } } } );

  ParameterMapType component3Parameters;
  component3Parameters[ "NameOfClass" ]    = { "ItkImageSinkComponent" };
  component3Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "ResultImageSink", component3Parameters );

  blueprint->SetComponent( "ResultDisplacementFieldSink", { { "NameOfClass", { "DisplacementFieldItkImageFilterSinkComponent" } },
                                                            { "Dimensionality", { "3" } },
                                                            { "PixelType", { "float" } } } );

  ParameterMapType component5Parameters;
  component5Parameters[ "NameOfClass" ]    = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component5Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "Metric", component5Parameters );

  ParameterMapType component6Parameters;
  component6Parameters[ "NameOfClass" ]    = { "ItkTransformDisplacementFilterComponent" };
  component6Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "TransformDisplacementFilter", component6Parameters );

  ParameterMapType component7Parameters;
  component7Parameters[ "NameOfClass" ]        = { "ItkGradientDescentOptimizerv4Component" };
  component7Parameters[ "NumberOfIterations" ] = { "1" };
  blueprint->SetComponent( "Optimizer", component7Parameters );

  blueprint->SetComponent( "ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } } } );

  blueprint->SetComponent( "Transform", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } } } );

  blueprint->SetComponent( "TransformResolutionAdaptor",
    { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
      { "ShrinkFactorsPerLevel", { "2", "1" } } } );

  blueprint->SetComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "ResampleFilter", "ResultImageSink", connection3Parameters );

  ParameterMapType connection4Parameters;
  connection4Parameters[ "NameOfInterface" ] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->SetConnection( "TransformDisplacementFilter", "ResultDisplacementFieldSink", connection4Parameters );

  ParameterMapType connection5Parameters;
  connection5Parameters[ "NameOfInterface" ] = { "itkMetricv4Interface" };
  blueprint->SetConnection( "Metric", "RegistrationMethod", connection5Parameters );

  blueprint->SetConnection( "FixedImageSource", "Transform", { {} } );
  blueprint->SetConnection( "Transform", "RegistrationMethod", { {} } );

  blueprint->SetConnection( "FixedImageSource", "TransformResolutionAdaptor", { {} } );
  blueprint->SetConnection( "TransformResolutionAdaptor", "RegistrationMethod", { {} } );
  blueprint->SetConnection( "Optimizer", "RegistrationMethod", { { "InternalComputationValueType", { "double" } } } );
  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { { keys::NameOfInterface, { "itkImageMovingInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->SetConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->SetConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface

  std::unique_ptr< NetworkBuilderBase > networkBuilder( new NetworkBuilder< RegisterComponents >( *logger ) );
  networkBuilder->AddBlueprint( blueprint );
  bool allUniqueComponents;
  EXPECT_NO_THROW( allUniqueComponents = networkBuilder->Configure() );
  EXPECT_TRUE( allUniqueComponents );
}
} // namespace selx
