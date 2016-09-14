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

#include "selxOverlord.h"

//#include "ComponentFactory.h"
#include "selxTransformComponent1.h"
#include "selxMetricComponent1.h"
#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

#include "selxRegisterComponentFactoriesByTypeList.h"

#include "selxDefaultComponents.h"


#include "gtest/gtest.h"

namespace selx
{
class OverlordTest : public ::testing::Test
{
public:

  //typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer            BlueprintPointerType;
  typedef Blueprint::ConstPointer       BlueprintConstPointerType;
  typedef Blueprint::ParameterMapType   ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;

  virtual void SetUp()
  {
    /** register all example components */
    ComponentFactory< TransformComponent1 >::RegisterOneFactory();
    ComponentFactory< MetricComponent1 >::RegisterOneFactory();
    ComponentFactory< GDOptimizer3rdPartyComponent >::RegisterOneFactory();
    ComponentFactory< GDOptimizer4thPartyComponent >::RegisterOneFactory();
    ComponentFactory< SSDMetric3rdPartyComponent >::RegisterOneFactory();
    ComponentFactory< SSDMetric4thPartyComponent >::RegisterOneFactory();

   

    /** make example blueprint configuration */
    blueprint = Blueprint::New();
    ParameterMapType metricComponentParameters;
    metricComponentParameters[ "NameOfClass" ] = { "MetricComponent1" };

    ParameterMapType transformComponentParameters;
    transformComponentParameters[ "NameOfClass" ] = { "TransformComponent1" };

    blueprint->AddComponent( "Metric", metricComponentParameters );
    blueprint->AddComponent( "Transform", transformComponentParameters );

    ParameterMapType metric2TransformConnectionParameters;
    metric2TransformConnectionParameters[ "NameOfInterface" ] = { "TransformedImageInterface" };

    blueprint->AddConnection( "Transform", "Metric", metric2TransformConnectionParameters );
  }


  virtual void TearDown()
  {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }


  //std::unique_ptr<Blueprint> blueprint(new Blueprint());
  BlueprintPointerType blueprint;
  //Overlord::Pointer overlord;
};

TEST_F( OverlordTest, Create )
{
  std::unique_ptr< Overlord > overlord( new Overlord() );
  EXPECT_NO_THROW( overlord->SetBlueprint( blueprint ) );
}

TEST_F( OverlordTest, Configure )
{
  std::unique_ptr< Overlord > overlord( new Overlord() );
  overlord->SetBlueprint( blueprint );
  bool allUniqueComponents;
  EXPECT_NO_THROW( allUniqueComponents = overlord->Configure() );
  EXPECT_TRUE( allUniqueComponents );
}

TEST_F( OverlordTest, Connect )
{
  std::unique_ptr< Overlord > overlord( new Overlord() );
  overlord->SetBlueprint( blueprint );
  EXPECT_NO_THROW( bool allUniqueComponents = overlord->Configure() );
  bool success;
  EXPECT_NO_THROW( success = overlord->ConnectComponents() );
}
TEST_F(OverlordTest, DeduceComponentsFromConnections)
{
  using CustomRegisterComponents = TypeList < 
    DisplacementFieldItkImageFilterSinkComponent< 3, double >,
    DisplacementFieldItkImageFilterSinkComponent< 3, float >,
    ItkImageSinkComponent< 3, double >,
    ItkImageSinkComponent< 3, float >,
    ItkImageSourceFixedComponent< 3, float >,
    ItkImageSourceMovingComponent< 3, float >,
    ItkImageSourceFixedComponent< 3, double >,
    ItkImageSourceMovingComponent< 3, double >,
    ItkImageRegistrationMethodv4Component< 3, double >,
    ItkImageRegistrationMethodv4Component< 3, float >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, float >,
    ItkAffineTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 3, double >,
    ItkTransformDisplacementFilterComponent< 3, float, double >,
    ItkTransformDisplacementFilterComponent< 3, double, double >,
    ItkResampleFilterComponent< 3, float, double >,
    ItkResampleFilterComponent< 3, double, double >> ;

  using RegisterComponents = list_append< DefaultComponents, CustomRegisterComponents>::type;

  RegisterFactoriesByTypeList<RegisterComponents>::Register();
  
  blueprint = Blueprint::New(); // override old blueprint

  blueprint->AddComponent("RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
  { "Dimensionality", { "3" } },
  { "NumberOfLevels", { "2" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } } });

  blueprint->AddComponent("FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } },
  { "Dimensionality", { "3" } },
  { "PixelType", { "float" } } });

  blueprint->AddComponent("MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } },
  { "Dimensionality", { "3" } },
  { "PixelType", { "float" } } });


  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  component3Parameters["Dimensionality"] = { "3" }; // should be derived from the outputs
  blueprint->AddComponent("ResultImageSink", component3Parameters);

  blueprint->AddComponent("ResultDisplacementFieldSink", { { "NameOfClass", { "DisplacementFieldItkImageFilterSinkComponent" } },
  { "Dimensionality", { "3" } },
  { "PixelType", { "float" } } });

  ParameterMapType component5Parameters;
  component5Parameters["NameOfClass"] = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component5Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  blueprint->AddComponent("Metric", component5Parameters);

  ParameterMapType component6Parameters;
  component6Parameters["NameOfClass"] = { "ItkTransformDisplacementFilterComponent" };
  component6Parameters["Dimensionality"] = { "3" }; // should be derived from the outputs
  blueprint->AddComponent("TransformDisplacementFilter", component6Parameters);

  ParameterMapType component7Parameters;
  component7Parameters["NameOfClass"] = { "ItkGradientDescentOptimizerv4Component" };
  component7Parameters["NumberOfIterations"] = { "1" };
  blueprint->AddComponent("Optimizer", component7Parameters);

  blueprint->AddComponent("ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } },
  { "Dimensionality", { "3" } } });

  blueprint->AddComponent("Transform", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } },
  { "Dimensionality", { "3" } } });

  blueprint->AddComponent("TransformResolutionAdaptor", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
  { "Dimensionality", { "3" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } } });

  blueprint->AddComponent("Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } });

  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "itkImageInterface" };
  blueprint->AddConnection("ResampleFilter", "ResultImageSink", connection3Parameters);

  ParameterMapType connection4Parameters;
  connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->AddConnection("TransformDisplacementFilter", "ResultDisplacementFieldSink", connection4Parameters);

  ParameterMapType connection5Parameters;
  connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection("Metric", "RegistrationMethod", connection5Parameters);

  blueprint->AddConnection("FixedImageSource", "Transform", { {} });
  blueprint->AddConnection("Transform", "RegistrationMethod", { {} });

  blueprint->AddConnection("FixedImageSource", "TransformResolutionAdaptor", { {} });
  blueprint->AddConnection("TransformResolutionAdaptor", "RegistrationMethod", { {} });
  blueprint->AddConnection("Optimizer", "RegistrationMethod", { { "InternalComputationValueType", {"double"} } });
  blueprint->AddConnection("RegistrationMethod", "TransformDisplacementFilter", { {} });
  blueprint->AddConnection("FixedImageSource", "TransformDisplacementFilter", { {} });
  blueprint->AddConnection("RegistrationMethod", "ResampleFilter", { {} });
  blueprint->AddConnection("FixedImageSource", "ResampleFilter", { {} });
  blueprint->AddConnection("MovingImageSource", "ResampleFilter", { {} });

  blueprint->AddConnection("RegistrationMethod", "Controller", { {} });          //RunRegistrationInterface
  blueprint->AddConnection("ResampleFilter", "Controller", { {} });              //ReconnectTransformInterface
  blueprint->AddConnection("TransformDisplacementFilter", "Controller", { {} }); //ReconnectTransformInterface

  std::unique_ptr< Overlord > overlord(new Overlord());
  overlord->SetBlueprint(blueprint);
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
}
} // namespace selx
