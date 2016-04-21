#include "selxBlueprint.h"

#include "gtest/gtest.h"

using namespace selx;

class BlueprintTest : public ::testing::Test {
public:

  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;

  virtual void SetUp() {
    parameterMap["NameOfClass"] = ParameterValueType(1, "TestClassName");
  }

  ParameterMapType parameterMap;
};

TEST_F( BlueprintTest, AddComponent )
{
  BlueprintPointerType blueprint;
  EXPECT_NO_THROW( blueprint = Blueprint::New() );

  ComponentIndexType index0;
  EXPECT_NO_THROW( index0 = blueprint->AddComponent( "MyComponentName" ) );

  ComponentIndexType index1;
  EXPECT_NO_THROW(index1 = blueprint->AddComponent("MyComponentName", parameterMap));
}

TEST_F( BlueprintTest, GetComponent ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent("MyComponentName", parameterMap);

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( "MyComponentName" ) );
  EXPECT_EQ( parameterMap["NameOfClass"], parameterMapTest["NameOfClass"] );
}

TEST_F( BlueprintTest, SetComponent ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent( "MyComponentName", parameterMap);

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW(blueprint->SetComponent("MyComponentName", parameterMap));
  EXPECT_NO_THROW(parameterMapTest = blueprint->GetComponent("MyComponentName"));
  EXPECT_EQ( parameterMap["NameOfClass"], parameterMapTest["NameOfClass"] );
}

// TODO: The final line segfaults because GetComponent does not check that the index actually
// actually exist. See explanation in selxBlueprint.h
// TEST_F( BlueprintTest, DeleteComponent ) 
// {
//   BlueprintPointerType blueprint = Blueprint::New();
//   ComponentIndexType index = blueprint->AddComponent( parameterMap );
//
//   ParameterMapType parameterMapTest;
//   EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
//   EXPECT_EQ( parameterMap["NameOfClass"], parameterMapTest["NameOfClass"] );
//
//   EXPECT_NO_THROW( blueprint->DeleteComponent( index ) );
//   EXPECT_ANY_THROW( parameterMapTest = blueprint->GetComponent( index ) );
// }

TEST_F( BlueprintTest, AddConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent( "Component0" );
  ComponentIndexType index1 = blueprint->AddComponent( "Component1" );
  ComponentIndexType index2 = blueprint->AddComponent( "Component2" );

  // Connection should not exist
  EXPECT_FALSE(blueprint->ConnectionExists("Component0", "Component1"));

  // Connection should be added
  EXPECT_TRUE(blueprint->AddConnection("Component0", "Component1"));

  // Connection should exist
  EXPECT_TRUE(blueprint->ConnectionExists("Component0", "Component1"));

  // Another connection between same components should not be added
  // (user should use SetComponent() instead)
  EXPECT_FALSE(blueprint->AddConnection("Component0", "Component1"));

  // Connection should be empty
  ParameterMapType parameterMapTest0;
  EXPECT_NO_THROW(parameterMapTest0 = blueprint->GetConnection("Component0", "Component1"));
  EXPECT_EQ( 0u, parameterMapTest0.size() );

  // Connection with properties should be added. Testing if properties was 
  // properly set requires a call to GetConnection() which is the responsibility
  // of the next test.
  ParameterMapType parameterMapTest1;
  EXPECT_TRUE(blueprint->AddConnection("Component1", "Component2", parameterMap));
}

TEST_F( BlueprintTest, GetConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent("Component0");
  ComponentIndexType index1 = blueprint->AddComponent("Component1");

  ParameterMapType parameterMapTest0;
  EXPECT_TRUE(blueprint->AddConnection("Component0", "Component1", parameterMap));
  EXPECT_NO_THROW(parameterMapTest0 = blueprint->GetConnection("Component0", "Component1"));
  EXPECT_EQ( parameterMap["NameOfClass"], parameterMapTest0["NameOfClass"] );
}

TEST_F( BlueprintTest, SetConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent("Component0");
  ComponentIndexType index1 = blueprint->AddComponent("Component1");
  blueprint->AddConnection("Component0", "Component1", parameterMap);

  ParameterMapType parameterMapTest0;
  parameterMapTest0 = blueprint->GetConnection("Component0", "Component1");
  EXPECT_EQ( parameterMap["NameOfClass"], parameterMapTest0["NameOfClass"] );

  ParameterMapType parameterMapTest1;
  parameterMapTest1["NameOfClass"] = ParameterValueType(1, "OtherName");
  EXPECT_TRUE(blueprint->SetConnection("Component0", "Component1", parameterMapTest1));

  ParameterMapType parameterMapTest2;
  EXPECT_NO_THROW(parameterMapTest2 = blueprint->GetConnection("Component0", "Component1"));
  EXPECT_EQ( parameterMapTest1["NameOfClass"], parameterMapTest2["NameOfClass"] );
}

TEST_F( BlueprintTest, DeleteConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent("Component0");
  ComponentIndexType index1 = blueprint->AddComponent("Component1");
  blueprint->AddConnection("Component0", "Component1");

  // Connection should exist
  EXPECT_TRUE(blueprint->ConnectionExists("Component0", "Component1"));

  // Connection be deleted
  EXPECT_TRUE(blueprint->DeleteConnection("Component0", "Component1"));

  // Connection should not exist 
  EXPECT_FALSE(blueprint->ConnectionExists("Component0", "Component1"));
}

TEST_F( BlueprintTest, WriteBlueprint ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  
  // create some made up configuration to show graphviz output
  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "MyMetric" };
  component0Parameters["Dimensionality"] = { "3" };
  component0Parameters["Kernel"] = { "5", "5", "5" };
  ComponentIndexType index0 = blueprint->AddComponent("Metric", component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "MyFiniteDifferenceCalculator" };
  component1Parameters["Delta"] = { "0.01" };
  ComponentIndexType index1 = blueprint->AddComponent("MetricGradient", component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "MyOptimizer" };
  ComponentIndexType index2 = blueprint->AddComponent("Optimizer", component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "MyTransform" };
  ComponentIndexType index3 = blueprint->AddComponent("Transform", component3Parameters);
  
  blueprint->AddConnection("Metric", "MetricGradient");
  blueprint->AddConnection("MetricGradient", "Optimizer");

  ParameterMapType connection0Parameters;
  // Example use case: The connection between the metric and optimizer should 
  // only be by "MetricValue", not by "MetricDerivative" as well. Since we want
  // to redirect the "MetricDerivative" through the MetricGradient component,
  // we need to specify "NameOfInterface" otherwise there is an ambiguity in 
  // which "MetricDerivative" to connect to the optimizer.
  
  connection0Parameters["NameOfInterface"] = { "MetricValue" };
  blueprint->AddConnection("Metric", "Optimizer", connection0Parameters);
  
  blueprint->AddConnection("MetricGradient", "Optimizer");
  blueprint->AddConnection("Optimizer", "Transform");
  blueprint->AddConnection("Transform", "Metric");

  EXPECT_NO_THROW( blueprint->WriteBlueprint( "blueprint.dot" ) );
}
