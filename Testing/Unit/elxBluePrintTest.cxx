#include "elxBlueprint.h"

#include "gtest/gtest.h"

namespace elx {

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

TEST_F( BlueprintTest, Add )
{
  BlueprintPointerType blueprint;
  EXPECT_NO_THROW( blueprint = Blueprint::New() );

  ComponentIndexType index0;
  EXPECT_NO_THROW( index0 = blueprint->AddComponent() );
  EXPECT_EQ( Blueprint::ComponentIndexType(0), index0 );

  ComponentIndexType index1;
  EXPECT_NO_THROW( index1 = blueprint->AddComponent( parameterMap ) );
  EXPECT_EQ( Blueprint::ComponentIndexType(1), index1 );

  Blueprint::ComponentIteratorPairType componentIterator = blueprint->GetComponentIterator();
}

TEST_F( BlueprintTest, Get ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent( parameterMap );
  EXPECT_EQ( Blueprint::ComponentIndexType(0), index );

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );
}

TEST_F( BlueprintTest, Delete ) 
{
  BlueprintPointerType blueprint = Blueprint::New();
  ComponentIndexType index = blueprint->AddComponent( parameterMap );

  ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );

  EXPECT_NO_THROW( blueprint->DeleteComponent( index ) );
  EXPECT_ANY_THROW( parameterMapTest = blueprint->GetComponent( index ) );
}

} // namespace elx
