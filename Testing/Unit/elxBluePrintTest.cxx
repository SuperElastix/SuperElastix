#include "elxBlueprint.h"
#include "elxComponentDescriptor.h"
#include "gtest/gtest.h"

namespace elx {

TEST( Blueprint, Instantiation )
{
  typedef Blueprint< ComponentDescriptor > BlueprintType;
  BlueprintType::Pointer blueprint;
  EXPECT_NO_THROW( blueprint = BlueprintType::New() );

  typedef BlueprintType::ComponentDescriptorType ComponentDescriptorType;
  ComponentDescriptorType::Pointer componentDescriptor;
  EXPECT_NO_THROW( componentDescriptor = ComponentDescriptorType::New() );

  typedef ComponentDescriptorType::ComponentNameType ComponentNameType;
  ComponentNameType componentName;
  EXPECT_NO_THROW( componentName = ComponentNameType("Metric") );
  EXPECT_NO_THROW( componentDescriptor->SetComponentName( componentName ) );

  EXPECT_NO_THROW( blueprint->TestFunction() );

  ASSERT_TRUE( true );
}

} // namespace elx