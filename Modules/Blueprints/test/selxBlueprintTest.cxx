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

#include "selxBlueprintImpl.h"
#include "selxBlueprint.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

using namespace selx;

class BlueprintTest : public ::testing::Test
{
public:

  typedef BlueprintImpl::ParameterMapType   ParameterMapType;
  typedef BlueprintImpl::ParameterValueType ParameterValueType;

  virtual void SetUp()
  {
    parameterMap[ "NameOfClass" ]        = ParameterValueType( 1, "TestClassName" );
    anotherParameterMap[ "NameOfClass" ] = ParameterValueType( 1, "AnotherTestClassName" );
    dataManager = DataManager::New();
  }

  typedef Blueprint::Pointer BlueprintPointer;

  DataManager::Pointer dataManager;
  ParameterMapType parameterMap;
  ParameterMapType anotherParameterMap;
};

TEST_F( BlueprintTest, SetGetDeleteComponent )
{
  auto blueprint = Blueprint::New();

  bool success0;
  EXPECT_NO_THROW( success0 = blueprint->SetComponent( "MyComponentName", parameterMap ) );
  EXPECT_TRUE( success0 );

  bool success1;
  EXPECT_NO_THROW( blueprint->SetComponent( "MyComponentName", parameterMap ) );
  EXPECT_NO_THROW( success1 = blueprint->SetComponent( "MyComponentName", anotherParameterMap ) );
  EXPECT_TRUE( success1 );
  EXPECT_EQ( anotherParameterMap, blueprint->GetComponent( "MyComponentName" ) );
}

TEST_F( BlueprintTest, BlueprintObjectSetGetDeleteComponent )
{
  BlueprintPointer blueprint;
  EXPECT_NO_THROW( blueprint = Blueprint::New() );

  bool success0;
  EXPECT_NO_THROW( success0 = blueprint->SetComponent( "MyComponentName", parameterMap ) );
  EXPECT_TRUE( success0 );

  bool success1;
  EXPECT_NO_THROW( blueprint->SetComponent( "MyComponentName", parameterMap ) );
  EXPECT_NO_THROW( success1 = blueprint->SetComponent( "MyComponentName", anotherParameterMap ) );
  EXPECT_TRUE( success1 );
  EXPECT_EQ( anotherParameterMap, blueprint->GetComponent( "MyComponentName" ) );
}

TEST_F( BlueprintTest, SetGetDeleteConnection )
{
  auto blueprint = Blueprint::New();

  blueprint->SetComponent( "Component0", parameterMap );
  blueprint->SetComponent( "Component1", parameterMap );
  blueprint->SetComponent( "Component2", parameterMap );

  // Connection should not exist
  bool connectionExists;
  EXPECT_NO_THROW( connectionExists = blueprint->ConnectionExists( "Component0", "Component1" ) );
  EXPECT_FALSE( connectionExists );

  // Connection should be set
  bool connectionSet;
  EXPECT_NO_THROW( connectionSet = blueprint->SetConnection( "Component0", "Component1", parameterMap ) );
  EXPECT_TRUE( connectionSet );

  // Connection should now exist
  bool connectionNowExists;
  EXPECT_NO_THROW( connectionNowExists = blueprint->ConnectionExists( "Component0", "Component1" ) );
  EXPECT_TRUE( connectionNowExists );

  // Connection should have parameter map
  ParameterMapType parameterMap0;
  EXPECT_NO_THROW( parameterMap0 = blueprint->GetConnection( "Component0", "Component1" ) );
  EXPECT_EQ( parameterMap[ "NameOfClass" ], parameterMap0[ "NameOfClass" ] );

  // Another parameter map should transparently added
  bool anotherParameterMapSet;
  EXPECT_NO_THROW( anotherParameterMapSet = blueprint->SetConnection( "Component0", "Component1", anotherParameterMap ) );
  EXPECT_TRUE( anotherParameterMapSet );
  EXPECT_EQ( anotherParameterMap, blueprint->GetConnection( "Component0", "Component1" ) );

  // Connection should be deleted
  bool connectionDeleted;
  EXPECT_NO_THROW( connectionDeleted = blueprint->DeleteConnection( "Component0", "Component1" ) );
  EXPECT_TRUE( connectionDeleted );

  // Connection should not exist
  EXPECT_FALSE( blueprint->ConnectionExists( "Component0", "Component1" ) );

  EXPECT_THROW( blueprint->GetConnection( "Component0", "Component1" ), std::runtime_error );
}

TEST_F( BlueprintTest, CopyConstuctor )
{
  auto baseBlueprint = Blueprint::New();
  baseBlueprint->SetComponent( "Component0", { { "OperationType", { "Transform" } } } );

  auto baseBlueprintImpl = baseBlueprint->GetBlueprintImpl();

  std::unique_ptr< BlueprintImpl > clonedBaseBlueprint;
  EXPECT_NO_THROW( clonedBaseBlueprint = std::make_unique< BlueprintImpl >( baseBlueprintImpl ) );

  EXPECT_NO_THROW( clonedBaseBlueprint->SetComponent( "Component1", { { "OperationType", { "Source" } }, { "Dimensionality", { "3" } } } ) );

  BlueprintImpl::ParameterMapType clonedComponent0;
  EXPECT_NO_THROW( clonedComponent0 = clonedBaseBlueprint->GetComponent( "Component0" ) );

  BlueprintImpl::ParameterMapType component1;
  EXPECT_THROW( component1 = baseBlueprint->GetComponent( "Component1" ), std::runtime_error );
}

TEST_F( BlueprintTest, Compose )
{
  auto baseBlueprint = Blueprint::New();

  baseBlueprint->SetComponent( "Component0", { { "OperationType", { "Transform" } } } );
  baseBlueprint->SetComponent( "Component1", { { "OperationType", { "Source" } }, { "Dimensionality", { "3" } } } );

  // compose-in a new 3rd component Component2
  auto nonConflictingBlueprint0 = Blueprint::New();
  
  nonConflictingBlueprint0->SetComponent( "Component2", { { "OperationType", { "Sink" } } } );

  EXPECT_TRUE( baseBlueprint->ComposeWith( nonConflictingBlueprint0 ) );
  EXPECT_STREQ( "Sink", baseBlueprint->GetComponent( "Component2" )[ "OperationType" ][ 0 ].c_str() );

  // compose-in additional properties of Component0 and Component1
  auto nonConflictingBlueprint1 = Blueprint::New();
  
  nonConflictingBlueprint1->SetComponent( "Component0", { { "TranformationGroup", { "Diffeomorphic" } }, { "PixelType", { "float" } } } );
  nonConflictingBlueprint1->SetComponent( "Component1", { { "NameOfClass", { "ImageSourceClass" } } } );

  EXPECT_TRUE( baseBlueprint->ComposeWith( nonConflictingBlueprint1 ) );
  EXPECT_STREQ( "Transform", baseBlueprint->GetComponent( "Component0" )[ "OperationType" ][ 0 ].c_str() );
  EXPECT_STREQ( "Diffeomorphic", baseBlueprint->GetComponent( "Component0" )[ "TranformationGroup" ][ 0 ].c_str() );
  EXPECT_STREQ( "ImageSourceClass", baseBlueprint->GetComponent( "Component1" )[ "NameOfClass" ][ 0 ].c_str() );

  // compose-in existing component with existing property key, but equal property value(s). Nothing happens actually (i.e. idempotency)
  auto nonConflictingBlueprint2 = Blueprint::New();

  nonConflictingBlueprint2->SetComponent( "Component0", { { "PixelType", { "float" } } } );

  EXPECT_TRUE( baseBlueprint->ComposeWith( nonConflictingBlueprint2 ) );

  // trying to overwrite properties fails
  auto conflictingBlueprint0 = Blueprint::New();
  conflictingBlueprint0->SetComponent( "Component1", { { "Dimensionality", { "2" } }, { "InternalComputationValueType", { "float" } } } );

  // Compose fails and returns false
  EXPECT_FALSE( baseBlueprint->ComposeWith( conflictingBlueprint0 ) );

  //baseBlueprint should not have been altered by a failing compose operation
  EXPECT_STREQ( "3", baseBlueprint->GetComponent( "Component1" )[ "Dimensionality" ][ 0 ].c_str() );

  EXPECT_EQ( 0, baseBlueprint->GetComponent( "Component1" ).count( "InternalComputationValueType" ) );
}
//TEST_F( BlueprintTest, WriteBlueprint )
//{
//  std::unique_ptr< BlueprintImpl > blueprint;
//  EXPECT_NO_THROW( blueprint = std::unique_ptr< BlueprintImpl >( new BlueprintImpl() ) );
//
//  // create some made up configuration to show graphviz output
//  ParameterMapType component0Parameters;
//  component0Parameters[ "NameOfClass" ]    = { "MyMetric" };
//  component0Parameters[ "Dimensionality" ] = { "3" };
//  component0Parameters[ "Kernel" ]         = { "5", "5", "5" };
//  blueprint->SetComponent( "Metric", component0Parameters );
//
//  ParameterMapType component1Parameters;
//  component1Parameters[ "NameOfClass" ] = { "MyFiniteDifferenceCalculator" };
//  component1Parameters[ "Delta" ]       = { "0.01" };
//  blueprint->SetComponent( "MetricGradient", component1Parameters );
//
//  ParameterMapType component2Parameters;
//  component2Parameters[ "NameOfClass" ] = { "MyOptimizer" };
//  blueprint->SetComponent( "Optimizer", component2Parameters );
//
//  ParameterMapType component3Parameters;
//  component3Parameters[ "NameOfClass" ] = { "MyTransform" };
//  blueprint->SetComponent( "Transform", component3Parameters );
//
//  blueprint->SetConnection( "Metric", "MetricGradient", parameterMap );
//  blueprint->SetConnection( "MetricGradient", "Optimizer", parameterMap );
//
//  ParameterMapType connection0Parameters;
//  // Example use case: The connection between the metric and optimizer should
//  // only be by "MetricValue", not by "MetricDerivative" as well. Since we want
//  // to redirect the "MetricDerivative" through the MetricGradient component,
//  // we need to specify "NameOfInterface" otherwise there is an ambiguity in
//  // which "MetricDerivative" to connect to the optimizer.
//
//  connection0Parameters[ "NameOfInterface" ] = { "MetricValue" };
//  blueprint->SetConnection( "Metric", "Optimizer", connection0Parameters );
//
//  blueprint->SetConnection( "MetricGradient", "Optimizer", parameterMap );
//  blueprint->SetConnection( "Optimizer", "Transform", parameterMap );
//  blueprint->SetConnection( "Transform", "Metric", parameterMap );
//
//  EXPECT_NO_THROW( blueprint->Write( "blueprint.dot" ) );
//}


TEST_F(BlueprintTest, ReadXML)
{
  auto blueprint = Blueprint::New();

  EXPECT_NO_THROW(blueprint->MergeFromFile(this->dataManager->GetConfigurationFile("itkv4_SVF_ANTsCC.xml")));
  blueprint->Write(this->dataManager->GetOutputFile("configurationReaderTest_itkv4_SVF_ANTsCC.xml.dot"));
}

TEST_F(BlueprintTest, ReadJson)
{
  auto blueprint = Blueprint::New();

  EXPECT_NO_THROW(blueprint->MergeFromFile(this->dataManager->GetConfigurationFile("itkv4_SVF_ANTsCC.json")));
  blueprint->Write(this->dataManager->GetOutputFile("configurationReaderTest_itkv4_SVF_ANTsCC.json.dot"));
}