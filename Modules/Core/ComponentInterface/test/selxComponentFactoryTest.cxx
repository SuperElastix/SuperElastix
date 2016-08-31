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

#include "selxTransformComponent1.h"
//#include "itkTransformComponent1Factory.h"

#include "selxMetricComponent1.h"
//#include "itkMetricComponent1Factory.h"

#include "selxComponentFactory.h"
#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

namespace selx
{
class ComponentFactoryTest : public ::testing::Test
{
public:

  typedef std::list< itk::LightObject::Pointer > RegisteredObjectsContainerType;
  typedef ComponentBase                          ComponentType;

  //typedef ComponentBase::CriteriaType CriteriaType;
  typedef ComponentBase::CriterionType      CriterionType;
  typedef ComponentBase::ParameterValueType ParameterValueType;
  //typedef std::map<std::string, std::string> CriteriaType;
  //typedef std::pair<std::string, std::string> CriterionType;

  typedef ComponentSelector::Pointer NodePointer;

  virtual void SetUp()
  {
  }


  virtual void TearDown()
  {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }


  // each node can hold multiple components (or none). Its the overlord's task to make it one per node.
  NodePointer Node1;
  NodePointer Node2;
};

TEST_F( ComponentFactoryTest, EmptyObjectFactoryBase )
{
  // The Component factory is inspired/based on the ImageIO factory.
  // In ITK Components (transformIO and imageIO) can be preregistered: CMake\UseITK.cmake sets up the IOFactory_Register_Manager
  // In Elastix (by Denis) GPU filters are preregistered using (simpleITK's) Typelists and Cmake
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1.

  RegisteredObjectsContainerType registeredComponents;

  // "When CMake is not used to register the Component classes, there are"
  EXPECT_NO_THROW( registeredComponents = itk::ObjectFactoryBase::CreateAllInstance( "ComponentBase" ) );
  // " 0 Component objects available to the Overlord."
  EXPECT_EQ( registeredComponents.size(), 0 );
}

TEST_F( ComponentFactoryTest, FilledObjectFactoryBase )
{
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1.
  RegisteredObjectsContainerType registeredComponents;

  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  //EXPECT_NO_THROW(TransformComponent1Factory::RegisterOneFactory());
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );
  //EXPECT_NO_THROW(MetricComponent1Factory::RegisterOneFactory());

  // After registering the TransformComponent1 and MetricComponent1object, there are
  EXPECT_NO_THROW( registeredComponents = itk::ObjectFactoryBase::CreateAllInstance( "ComponentBase" ) );
  // " 2 Component objects available to the Overlord."
  EXPECT_EQ( registeredComponents.size(), 2 );
}

TEST_F( ComponentFactoryTest, SetEmptyCriteria )
{
  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );

  CriterionType emptyCriterion; // = CriterionType();

  ASSERT_NO_THROW( Node1 = ComponentSelector::New() );

  EXPECT_NO_THROW( Node1->AddCriterion( emptyCriterion ) );
  ComponentType::Pointer Node1Component;
  EXPECT_NO_THROW( Node1Component = Node1->GetComponent() );

  //Too few criteria means no Component could be selected."
  EXPECT_TRUE( Node1Component.IsNull() );
}

TEST_F( ComponentFactoryTest, SetSufficientCriteria )
{
  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );

  CriterionType criterion2 = { "ComponentInput", { "Transform" } };
  ASSERT_NO_THROW( Node2 = ComponentSelector::New() );

  ASSERT_NO_THROW( Node2->AddCriterion( criterion2 ) );
  ComponentType::Pointer Node2Component;
  EXPECT_NO_THROW( Node2Component = Node2->GetComponent() );

  //Sufficient criteria means one Component was selected."
  EXPECT_FALSE( Node2Component.IsNull() );
  //Based on the criteria MetricComponent1 should be selected
  EXPECT_STREQ( Node2Component->GetNameOfClass(), "MetricComponent1" );
}
TEST_F( ComponentFactoryTest, AddCriteria )
{
  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );

  CriterionType nonSelectiveCriterion( "ComponentProperty", { "SomeProperty" } );

  CriterionType criterion1( { "ComponentOutput", { "Transform" } } );

  Node1 = ComponentSelector::New();

  EXPECT_NO_THROW( Node1->AddCriterion( nonSelectiveCriterion ) );
  ComponentType::Pointer Node1Component;

  EXPECT_TRUE( Node1->HasMultipleComponents() );
  EXPECT_NO_THROW( Node1Component = Node1->GetComponent() );
  //Unsufficient criteria means no Component was selected."
  EXPECT_TRUE( Node1Component.IsNull() );

  EXPECT_NO_THROW( Node1->AddCriterion( criterion1 ) );
  EXPECT_NO_THROW( Node1Component = Node1->GetComponent() );

  //Sufficient criteria means one Component was selected."
  EXPECT_FALSE( Node1->HasMultipleComponents() );
  EXPECT_FALSE( Node1Component.IsNull() );
  //Based on the criteria TransformComponent1 should be selected
  EXPECT_STREQ( Node1Component->GetNameOfClass(), "TransformComponent1" );
}

TEST_F( ComponentFactoryTest, InterfacedObjects )
{
  RegisteredObjectsContainerType registeredComponents;
  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< GDOptimizer3rdPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< GDOptimizer4thPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< SSDMetric3rdPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< SSDMetric4thPartyComponent >::RegisterOneFactory() );

  EXPECT_NO_THROW( registeredComponents = itk::ObjectFactoryBase::CreateAllInstance( "ComponentBase" ) );
  // " 6 Component objects available to the Overlord."
  EXPECT_EQ( registeredComponents.size(), 6 );

  
  NodePointer Node3 = ComponentSelector::New();
  Node3->AddAcceptingInterfaceCriteria({ { "NameOfInterface", "MetricDerivativeInterface" } });
   
  ComponentType::Pointer Node3Component;
  EXPECT_NO_THROW( Node3Component = Node3->GetComponent() );
  EXPECT_STREQ( Node3Component->GetNameOfClass(), "GDOptimizer3rdPartyComponent" );

  NodePointer Node4 = ComponentSelector::New();
  Node4->AddProvidingInterfaceCriteria({ { "NameOfInterface", "MetricDerivativeInterface" } });
  ComponentType::Pointer Node4Component;
  EXPECT_NO_THROW( Node4Component = Node4->GetComponent() );
  EXPECT_STREQ( Node4Component->GetNameOfClass(), "SSDMetric3rdPartyComponent" );
}

TEST_F( ComponentFactoryTest, UnknownComponent )
{
  // Fill our component database with some components
  RegisteredObjectsContainerType registeredComponents;
  EXPECT_NO_THROW( ComponentFactory< TransformComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< MetricComponent1 >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< GDOptimizer3rdPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< GDOptimizer4thPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< SSDMetric3rdPartyComponent >::RegisterOneFactory() );
  EXPECT_NO_THROW( ComponentFactory< SSDMetric4thPartyComponent >::RegisterOneFactory() );

  // Setup the criterion for a component that does not exist in our data base
  CriterionType criterion( { "NameOfClass", { "DoYouHaveThisComponent?" } } );

  NodePointer Node = ComponentSelector::New();
  Node->AddCriterion( criterion );
  ComponentType::Pointer NodeComponent;

  // we expect 0 components
  EXPECT_FALSE( Node->HasMultipleComponents() );
  EXPECT_TRUE( Node->GetComponent().IsNull() );
}
} // namespace selx
