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

// TODO: The final line segfaults since at this point GetComponent has no way
// of checking that the index actually exist. See explanation in elxBlueprint.h
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

} // namespace elx
