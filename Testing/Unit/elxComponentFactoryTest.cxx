#include "gtest/gtest.h"

#include "itkTransformComponent1.h"
#include "itkTransformComponent1Factory.h"

#include "itkMetricComponent1.h"
#include "itkMetricComponent1Factory.h"

#include "ComponentFactory.h"
#include "GDOptimizer3rdPartyComponent.h"

namespace selx{

class ComponentFactoryTest : public ::testing::Test {
public:
  typedef std::list< itk::LightObject::Pointer > RegisteredObjectsContainerType;
  typedef ComponentBase       ComponentType;
  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  typedef ComponentSelector::Pointer NodePointer;

  virtual void SetUp() 
  {
  }
  // each node can hold multiple components (or none). Its the overlord's task to make it one per node.
  NodePointer Node1;
  NodePointer Node2;
};

TEST_F(ComponentFactoryTest, EmptyObjectFactoryBase)
{
  // The Component factory is inspired/based on the ImageIO factory. 
  // In ITK Components (transformIO and imageIO) can be preregistered: CMake\UseITK.cmake sets up the IOFactory_Register_Manager
  // In Elastix (by Denis) GPU filters are preregistered using (simpleITK's) Typelists and Cmake
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 

  RegisteredObjectsContainerType registeredComponents;

  // "When CMake is not used to register the Component classes, there are"
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("itkComponentBase"));
  // " 0 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 0);

}

TEST_F(ComponentFactoryTest, FilledObjectFactoryBase)
{
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 
  RegisteredObjectsContainerType registeredComponents;

  EXPECT_NO_THROW(TransformComponent1Factory::RegisterOneFactory());
  EXPECT_NO_THROW(MetricComponent1Factory::RegisterOneFactory());

  // After registering the TransformComponent1 and MetricComponent1object, there are
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("itkComponentBase"));
  // " 2 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 2);
}

TEST_F(ComponentFactoryTest, SetEmptyCriteria)
{

  CriteriaType emptyCriteria;

  ASSERT_NO_THROW(Node1 = ComponentSelector::New());

  EXPECT_NO_THROW(Node1->SetCriteria(emptyCriteria));
  ComponentType::Pointer Node1Component;
  EXPECT_NO_THROW(Node1Component = Node1->GetComponent());

  //Too few criteria means no Component could be selected."
  EXPECT_TRUE(Node1Component.IsNull());
}

TEST_F(ComponentFactoryTest, SetSufficientCriteria)
{

  CriteriaType criteria2;
  criteria2["ComponentInput"] = "Transform";
  //criteria1.insert(CriteriumType("ComponentInput", "Metric"));
  ASSERT_NO_THROW(Node2 = ComponentSelector::New());

  Node2->SetCriteria(criteria2);
  ComponentType::Pointer Node2Component;
  EXPECT_NO_THROW(Node2Component = Node2->GetComponent());

  //Sufficient criteria means one Component was selected."
  EXPECT_FALSE(Node2Component.IsNull());
  //Based on the criteria MetricComponent1 should be selected
  EXPECT_STREQ(Node2Component->GetNameOfClass(), "MetricComponent1");

}
TEST_F(ComponentFactoryTest, AddCriteria)
{
  CriteriaType emptyCriteria;
  CriteriaType criteria1;
  //criteria1.insert(CriteriumType("ComponentOutput","Metric")); 
  criteria1["ComponentOutput"] = "Transform";
  //criteria1.insert(CriteriumType("ComponentInput", "Metric"));
  Node1 = ComponentSelector::New();

  Node1->SetCriteria(emptyCriteria);
  EXPECT_NO_THROW(Node1->AddCriteria(criteria1));
  ComponentType::Pointer Node1Component;
  EXPECT_NO_THROW(Node1Component = Node1->GetComponent());

  //Sufficient criteria means one Component was selected."
  EXPECT_FALSE(Node1Component.IsNull());
  //Based on the criteria TransformComponent1 should be selected
  EXPECT_STREQ(Node1Component->GetNameOfClass(), "TransformComponent1");
}

TEST_F(ComponentFactoryTest, InterfacedObjects)
{
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 
  RegisteredObjectsContainerType registeredComponents;

  EXPECT_NO_THROW(ComponentFactory<GDOptimizer3rdPartyComponent>::RegisterOneFactory());
  // After registering the TransformComponent1 and MetricComponent1object, there are
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("itkComponentBase"));
  // " 2 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 3);
}




} // namespace elx
