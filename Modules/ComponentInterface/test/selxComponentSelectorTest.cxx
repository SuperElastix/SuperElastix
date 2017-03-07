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

#include "gtest/gtest.h"

#include "selxComponentSelector.h"
#include "selxTypeList.h"
#include "selxTransformComponent1.h"
#include "selxMetricComponent1.h"

#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

namespace selx
{
class ComponentSelectorTest : public ::testing::Test
{
public:

  typedef ComponentBase ComponentType;

  //typedef ComponentBase::CriteriaType CriteriaType;
  typedef ComponentBase::CriterionType      CriterionType;
  typedef ComponentBase::ParameterValueType ParameterValueType;
  //typedef std::map<std::string, std::string> CriteriaType;
  //typedef std::pair<std::string, std::string> CriterionType;
  typedef TypeList< > EmptyComponentList;
  // each node can hold multiple components (or none). Its the NetworkBuilder's task to make it one per node.
  //typedef ComponentSelector<EmptyComponentList>::Pointer NodePointer;

  using SmallComponentList = TypeList< TransformComponent1, MetricComponent1 >;

  using BigComponentList
      = TypeList< TransformComponent1, MetricComponent1, GDOptimizer3rdPartyComponent, GDOptimizer4thPartyComponent, SSDMetric3rdPartyComponent,
    SSDMetric4thPartyComponent >;

  virtual void SetUp()
  {
  }


  virtual void TearDown()
  {
  }
};

TEST_F( ComponentSelectorTest, EmptyComponentList )
{
  auto componentSelector = std::make_shared< ComponentSelector< EmptyComponentList >>( "nameless" );

  // " 0 Component objects available to the NetworkBuilder."
  EXPECT_EQ( componentSelector->NumberOfComponents(), 0 );
}

TEST_F( ComponentSelectorTest, FilledComponentList )
{
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1.
  auto componentSelector = std::make_shared< ComponentSelector< TypeList< TransformComponent1, MetricComponent1 >>>( "nameless" );

  // After registering the TransformComponent1 and MetricComponent1object, there are
  // " 2 Component objects available to the NetworkBuilder."
  EXPECT_EQ( componentSelector->NumberOfComponents(), 2 );
}

TEST_F( ComponentSelectorTest, SetEmptyCriteria )
{
  auto componentSelector = std::make_shared< ComponentSelector< SmallComponentList >>( "nameless" );

  CriterionType emptyCriterion; // = CriterionType();

  EXPECT_NO_THROW( componentSelector->AddCriterion( emptyCriterion ) );
  ComponentType::Pointer component;
  EXPECT_NO_THROW( component = componentSelector->GetComponent() );

  //Too few criteria means no Component could be selected."
  EXPECT_FALSE( component );
}

TEST_F( ComponentSelectorTest, SetSufficientCriteria )
{
  auto componentSelector = std::make_shared< ComponentSelector< SmallComponentList >>( "nameless" );

  CriterionType criterion = { "ComponentInput", { "Transform" } };

  ASSERT_NO_THROW( componentSelector->AddCriterion( criterion ) );
  ComponentType::Pointer component;
  EXPECT_NO_THROW( component = componentSelector->GetComponent() );

  //Based on the criteria MetricComponent1 should be selected
  EXPECT_TRUE( component->MeetsCriterion( { "NameOfClass", { "MetricComponent1" } } ) );
}
TEST_F( ComponentSelectorTest, AddCriteria )
{
  auto componentSelector = std::make_shared< ComponentSelector< SmallComponentList >>( "nameless" );

  CriterionType nonSelectiveCriterion( { "ComponentProperty", { "SomeProperty" } } );

  CriterionType criterion( { "ComponentOutput", { "Transform" } } );

  EXPECT_NO_THROW( componentSelector->AddCriterion( nonSelectiveCriterion ) );
  ComponentType::Pointer component;

  EXPECT_TRUE( componentSelector->NumberOfComponents() > 1 );
  EXPECT_NO_THROW( component = componentSelector->GetComponent() );
  //Insufficient criteria means no Component was selected."
  EXPECT_FALSE( component );

  EXPECT_NO_THROW( componentSelector->AddCriterion( criterion ) );
  EXPECT_NO_THROW( component = componentSelector->GetComponent() );

  //Sufficient criteria means one Component was selected."
  EXPECT_FALSE( componentSelector->NumberOfComponents() > 1 );

  //Based on the criteria TransformComponent1 should be selected
  EXPECT_TRUE( component->MeetsCriterion( { "NameOfClass", { "TransformComponent1" } } ) );
}

TEST_F( ComponentSelectorTest, InterfacedObjects )
{
  auto componentSelectorA = std::make_shared< ComponentSelector< BigComponentList >>( "nameless" );
  // " 6 Component objects available to the NetworkBuilder."
  EXPECT_EQ( componentSelectorA->NumberOfComponents(), 6 );

  componentSelectorA->AddAcceptingInterfaceCriteria( { { "NameOfInterface", "MetricDerivativeInterface" } } );

  ComponentType::Pointer componentA;
  EXPECT_NO_THROW( componentA = componentSelectorA->GetComponent() );
  EXPECT_TRUE( componentA->MeetsCriterion( { "NameOfClass", { "GDOptimizer3rdPartyComponent" } } ) );

  auto componentSelectorB = std::make_shared< ComponentSelector< BigComponentList >>( "nameless" );
  componentSelectorB->AddProvidingInterfaceCriteria( { { "NameOfInterface", "MetricDerivativeInterface" } } );
  ComponentType::Pointer componentB;
  EXPECT_NO_THROW( componentB = componentSelectorB->GetComponent() );
  EXPECT_TRUE( componentB->MeetsCriterion( { "NameOfClass", { "SSDMetric3rdPartyComponent" } } ) );
}

TEST_F( ComponentSelectorTest, UnknownComponent )
{
  // Fill our component database with some components
  auto componentSelector = std::make_shared< ComponentSelector< BigComponentList >>( "nameless" );

  // Setup the criterion for a component that does not exist in our data base
  CriterionType criterion( { "NameOfClass", { "DoYouHaveThisComponent?" } } );

  componentSelector->AddCriterion( criterion );
  ComponentType::Pointer component;

  // we expect 0 components
  EXPECT_TRUE( componentSelector->NumberOfComponents() == 0 );
  EXPECT_FALSE( componentSelector->GetComponent() );
}
} // namespace selx
