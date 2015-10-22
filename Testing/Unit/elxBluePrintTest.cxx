#include "elxBlueprint.h"

#include "gtest/gtest.h"

namespace elx {

TEST( Blueprint, Instantiation )
{
  typedef Blueprint BlueprintType;
  BlueprintType::Pointer blueprint;
  EXPECT_NO_THROW( blueprint = BlueprintType::New() );
  
  BlueprintType::ParameterMapType parameterMap;
  EXPECT_NO_THROW( parameterMap["ComponentName"] = BlueprintType::ParameterValueType(1, "AdvancedMeanSquares") );

  BlueprintType::ComponentIndexType index;
  EXPECT_NO_THROW( index = blueprint->AddComponent() );
  EXPECT_EQ( BlueprintType::ComponentIndexType(0), index );

  EXPECT_NO_THROW( index = blueprint->AddComponent( parameterMap ) );
  EXPECT_EQ( BlueprintType::ComponentIndexType(1), index );

  BlueprintType::ParameterMapType parameterMapTest;
  EXPECT_NO_THROW( parameterMapTest = blueprint->GetComponent( index ) );
  EXPECT_EQ( parameterMap["ComponentName"], parameterMapTest["ComponentName"] );

  blueprint[index];
}

} // namespace elx
