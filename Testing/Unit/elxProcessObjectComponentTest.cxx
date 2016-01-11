#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "Overlord.h"

#include "gtest/gtest.h"

namespace selx {

class ProcessObjectTest : public ::testing::Test {
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;

  virtual void SetUp() {
    /** register all example components */
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent>::RegisterOneFactory();

    /** make example blueprint configuration */
    blueprint = Blueprint::New();
    ParameterMapType componentParameters;
    componentParameters["NameOfClass"] = ParameterValueType(1, "ItkSmoothingRecursiveGaussianImageFilterComponent");

    ComponentIndexType index0 = blueprint->AddComponent(componentParameters);
    ComponentIndexType index1 = blueprint->AddComponent(componentParameters);
    
    ParameterMapType connectionParameters;
    connectionParameters["NameOfInterface"] = ParameterValueType(1, "itkProcessObjectInterface");

    //TODO: check direction
    blueprint->AddConnection(index1, index0, connectionParameters);

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;
};

TEST_F(ProcessObjectTest, Configure)
{
  overlord = Overlord::New();
  overlord->SetBlueprint(blueprint);
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
}

} // namespace elx
