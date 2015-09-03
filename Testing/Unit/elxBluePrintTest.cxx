#include "elxBlueprint.h"
#include "elxComponentDescriptor.h"
#include "gtest/gtest.h"

namespace elx {

TEST( Blueprint, Instantiation )
{
  typedef Blueprint< ComponentDescriptor >  BlueprintType;
  BlueprintType::Pointer blueprint = BlueprintType::New();

  typedef BlueprintType::ComponentDescriptorType ComponentDescriptorType;
  ComponentDescriptorType::Pointer componentDescriptor = ComponentDescriptorType::New();

  typedef ComponentDescriptorType::ComponentNameType ComponentNameType;
  ComponentNameType componentName = ComponentNameType("Metric");
  componentDescriptor->SetComponentName( componentName );

  // We would like to save all data in the graph itself, but the following is 
  // not possible because the component (of type itk::LightObject has private copy 
  // constructor. How do we save data in graph and make this as itk-like as possible?
  // Ideally when a component descriptor is changed the blueprint calls Modified() on itself
  // blueprint->AddComponent( (*componentDescriptor) );

  ASSERT_TRUE( true );
}

} // namespace elx