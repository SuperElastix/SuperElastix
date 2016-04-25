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
    parameterMap["ComponentName"] = ParameterValueType(1, "TestName");
  }

  ParameterMapType parameterMap;
};

TEST_F( BlueprintTest, AddComponent )
{
  BlueprintPointerType blueprint;
  EXPECT_NO_THROW( blueprint = Blueprint::New() );

  ComponentIndexType index0;
  EXPECT_NO_THROW( index0 = blueprint->AddComponent() );

  ComponentIndexType index1;
  EXPECT_NO_THROW( index1 = blueprint->AddComponent( parameterMap ) );
}

TEST_F( BlueprintTest, GetComponent ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent( parameterMap );

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );
}

TEST_F( BlueprintTest, SetComponent ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent( parameterMap );

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( blueprint->SetComponent( index, parameterMap ) );
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );
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
//   EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );
//
//   EXPECT_NO_THROW( blueprint->DeleteComponent( index ) );
//   EXPECT_ANY_THROW( parameterMapTest = blueprint->GetComponent( index ) );
// }

TEST_F( BlueprintTest, AddConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent();
  ComponentIndexType index1 = blueprint->AddComponent();
  ComponentIndexType index2 = blueprint->AddComponent();

  // Connection should not exist
  EXPECT_FALSE( blueprint->ConnectionExists( index0, index1 ) );

  // Connection should be added
  EXPECT_TRUE( blueprint->AddConnection( index0, index1 ) );

  // Connection should exist
  EXPECT_TRUE( blueprint->ConnectionExists( index0, index1 ) );

  // Another connection between same components should not be added
  // (user should use SetComponent() instead)
  EXPECT_FALSE( blueprint->AddConnection( index0, index1 ) );

  // Connection should be empty
  ParameterMapType parameterMapTest0;
  EXPECT_NO_THROW( parameterMapTest0 = blueprint->GetConnection( index0, index1 ) );
  EXPECT_EQ( 0u, parameterMapTest0.size() );

  // Connection with properties should be added. Testing if properties was 
  // properly set requires a call to GetConnection() which is the responsibility
  // of the next test.
  ParameterMapType parameterMapTest1;
  EXPECT_TRUE( blueprint->AddConnection( index1, index2, parameterMap ) );
}

TEST_F( BlueprintTest, GetConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent();
  ComponentIndexType index1 = blueprint->AddComponent();

  ParameterMapType parameterMapTest0;
  EXPECT_TRUE( blueprint->AddConnection( index0, index1, parameterMap ) );
  EXPECT_NO_THROW( parameterMapTest0 = blueprint->GetConnection( index0, index1 ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest0["ComponentName"] );
}

TEST_F( BlueprintTest, SetConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent();
  ComponentIndexType index1 = blueprint->AddComponent();
  blueprint->AddConnection( index0, index1, parameterMap );

  ParameterMapType parameterMapTest0;
  parameterMapTest0 = blueprint->GetConnection( index0, index1 );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest0["ComponentName"] );

  ParameterMapType parameterMapTest1;
  parameterMapTest1["ComponentName"] = ParameterValueType(1, "OtherName");
  EXPECT_TRUE( blueprint->SetConnection( index0, index1, parameterMapTest1 ) );

  ParameterMapType parameterMapTest2;
  EXPECT_NO_THROW( parameterMapTest2 = blueprint->GetConnection( index0, index1 ) );
  EXPECT_EQ( parameterMapTest1["ComponentName"], parameterMapTest2["ComponentName"] );
}

TEST_F( BlueprintTest, DeleteConnection )
{
  BlueprintPointerType blueprint = Blueprint::New();

  ComponentIndexType index0 = blueprint->AddComponent();
  ComponentIndexType index1 = blueprint->AddComponent();
  blueprint->AddConnection( index0, index1 );

  // Connection should exist
  EXPECT_TRUE( blueprint->ConnectionExists( index0, index1 ) );

  // Connection be deleted
  EXPECT_TRUE( blueprint->DeleteConnection( index0, index1 ) );

  // Connection should not exist 
  EXPECT_FALSE( blueprint->ConnectionExists( index0, index1 ) );
}

TEST_F( BlueprintTest, WriteBlueprint ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  
  // create some made up configuration to show graphviz output
  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "MyMetric" };
  component0Parameters["Dimensionality"] = { "3" };
  component0Parameters["Kernel"] = { "5", "5", "5" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "MyFiniteDifferenceCalculator" };
  component1Parameters["Delta"] = { "0.01" };
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "MyOptimizer" };
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "MyTransform" };
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);
  
  blueprint->AddConnection(index0, index1);
  blueprint->AddConnection(index1, index2);

  ParameterMapType connection0Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  connection0Parameters["NameOfInterface"] = { "MetricValue" };
  blueprint->AddConnection(index0, index2, connection0Parameters);

  blueprint->AddConnection( index2, index3 );
  blueprint->AddConnection( index3, index0 );

  EXPECT_NO_THROW( blueprint->WriteBlueprint( "blueprint.dot" ) );
}
