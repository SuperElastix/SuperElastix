#include "gtest/gtest.h"

#include "TransformComponent1.h"
//#include "itkTransformComponent1Factory.h"

#include "MetricComponent1.h"
//#include "itkMetricComponent1Factory.h"

#include "ComponentFactory.h"
#include "GDOptimizer3rdPartyComponent.h"
#include "GDOptimizer4thPartyComponent.h"
#include "SSDMetric3rdPartyComponent.h"
#include "SSDMetric4thPartyComponent.h"

namespace selx{

class ComponentFactoryTest : public ::testing::Test {
public:
  typedef std::list< itk::LightObject::Pointer > RegisteredObjectsContainerType;
  typedef ComponentBase       ComponentType;

  typedef ComponentBase::CriteriaType CriteriaType;
  typedef ComponentBase::CriterionType CriterionType;
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

TEST_F(ComponentFactoryTest, EmptyObjectFactoryBase)
{
  // The Component factory is inspired/based on the ImageIO factory. 
  // In ITK Components (transformIO and imageIO) can be preregistered: CMake\UseITK.cmake sets up the IOFactory_Register_Manager
  // In Elastix (by Denis) GPU filters are preregistered using (simpleITK's) Typelists and Cmake
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 

  RegisteredObjectsContainerType registeredComponents;

  // "When CMake is not used to register the Component classes, there are"
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("ComponentBase"));
  // " 0 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 0);

}

TEST_F(ComponentFactoryTest, FilledObjectFactoryBase)
{
  // In this test we manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 
  RegisteredObjectsContainerType registeredComponents;

  EXPECT_NO_THROW(ComponentFactory<TransformComponent1>::RegisterOneFactory());
  //EXPECT_NO_THROW(TransformComponent1Factory::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<MetricComponent1>::RegisterOneFactory());
  //EXPECT_NO_THROW(MetricComponent1Factory::RegisterOneFactory());

  // After registering the TransformComponent1 and MetricComponent1object, there are
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("ComponentBase"));
  // " 2 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 2);
}

TEST_F(ComponentFactoryTest, SetEmptyCriteria)
{
  EXPECT_NO_THROW(ComponentFactory<TransformComponent1>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<MetricComponent1>::RegisterOneFactory());

  CriteriaType emptyCriteria; // = CriteriaType();

  ASSERT_NO_THROW(Node1 = ComponentSelector::New());

  EXPECT_NO_THROW(Node1->SetCriteria(emptyCriteria));
  ComponentType::Pointer Node1Component;
  EXPECT_NO_THROW(Node1Component = Node1->GetComponent());

  //Too few criteria means no Component could be selected."
  EXPECT_TRUE(Node1Component.IsNull());
}

TEST_F(ComponentFactoryTest, SetSufficientCriteria)
{
  EXPECT_NO_THROW(ComponentFactory<TransformComponent1>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<MetricComponent1>::RegisterOneFactory());

  CriteriaType criteria2;
  //criteria2["ComponentInput"] = "Transform";
  criteria2.insert(CriterionType("ComponentInput", ParameterValueType(1,"Transform")));
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
  EXPECT_NO_THROW(ComponentFactory<TransformComponent1>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<MetricComponent1>::RegisterOneFactory());

  CriteriaType emptyCriteria;
  CriteriaType criteria1;
  //criteria1.insert(CriterionType("ComponentOutput","Metric")); 
  //criteria1["ComponentOutput"] = "Transform";
  criteria1.insert(CriterionType("ComponentOutput", ParameterValueType(1, "Transform")));
  //criteria1.insert(CriterionType("ComponentInput", "Metric"));
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
  RegisteredObjectsContainerType registeredComponents;
  EXPECT_NO_THROW(ComponentFactory<TransformComponent1>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<MetricComponent1>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<GDOptimizer3rdPartyComponent>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<GDOptimizer4thPartyComponent>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<SSDMetric3rdPartyComponent>::RegisterOneFactory());
  EXPECT_NO_THROW(ComponentFactory<SSDMetric4thPartyComponent>::RegisterOneFactory());
  
  EXPECT_NO_THROW(registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("ComponentBase"));
  // " 6 Component objects available to the Overlord."
  EXPECT_EQ(registeredComponents.size(), 6);

  CriteriaType criteria3;
  // Criteria could be name or other properties
  //criteria3["NameOfClass"] = "GDOptimizer3rdPartyComponent";
  //criteria3["HasAcceptingInterface"] = "MetricDerivativeInterface";
  criteria3.insert(CriterionType("HasAcceptingInterface", ParameterValueType(1, "MetricDerivativeInterface")));
  NodePointer Node3 = ComponentSelector::New();
  Node3->SetCriteria(criteria3);
  ComponentType::Pointer Node3Component;
  EXPECT_NO_THROW(Node3Component = Node3->GetComponent());
  EXPECT_STREQ(Node3Component->GetNameOfClass(), "GDOptimizer3rdPartyComponent");

  CriteriaType criteria4;
  // Criteria could be name or other properties
  //criteria4["NameOfClass"] = "GDOptimizer4thPartyComponent";
  criteria4.insert(CriterionType("NameOfClass", ParameterValueType(1, "GDOptimizer4thPartyComponent")));
  //criteria3["HasDerivative"] = "True";  
  
  NodePointer Node4 = ComponentSelector::New();
  Node4->SetCriteria(criteria4);
  ComponentType::Pointer Node4Component;
  EXPECT_NO_THROW(Node4Component = Node4->GetComponent());
  EXPECT_STREQ(Node4Component->GetNameOfClass(), "GDOptimizer4thPartyComponent");

  CriteriaType criteria5;
  // Criteria could be name or other properties
  //criteria5["NameOfClass"] = "SSDMetric3rdPartyComponent";
  
  //criteria5["HasProvidingInterface"] = "MetricDerivativeInterface";
  criteria5.insert(CriterionType("HasProvidingInterface", ParameterValueType(1, "MetricDerivativeInterface")));
  //criteria3["HasDerivative"] = "True";  
  NodePointer Node5 = ComponentSelector::New();
  Node5->SetCriteria(criteria5);
  ComponentType::Pointer Node5Component;
  EXPECT_NO_THROW(Node5Component = Node5->GetComponent());
  EXPECT_STREQ(Node5Component->GetNameOfClass(), "SSDMetric3rdPartyComponent");

  CriteriaType criteria6;
  // Criteria could be name or other properties
  //criteria6["NameOfClass"] = "SSDMetric4thPartyComponent";
  criteria6.insert(CriterionType("NameOfClass", ParameterValueType(1, "SSDMetric4thPartyComponent")));
  //criteria3["HasDerivative"] = "True";  
  NodePointer Node6 = ComponentSelector::New();
  Node6->SetCriteria(criteria6);
  ComponentType::Pointer Node6Component;
  EXPECT_NO_THROW(Node6Component = Node6->GetComponent());
  EXPECT_STREQ(Node6Component->GetNameOfClass(), "SSDMetric4thPartyComponent");
  
}




} // namespace elx
