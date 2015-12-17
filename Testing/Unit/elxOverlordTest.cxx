#include "Overlord.h"

#include "gtest/gtest.h"

namespace selx {

class OverlordTest : public ::testing::Test {
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;

  virtual void SetUp() {
    blueprint = Blueprint::New();
    ParameterMapType metricComponentParameters;
    metricComponentParameters["NameOfClass"] = ParameterValueType(1, "MetricComponent1");

    ParameterMapType transformComponentParameters;
    transformComponentParameters["NameOfClass"] = ParameterValueType(1, "TransformComponent1");

    ComponentIndexType index0 = blueprint->AddComponent(metricComponentParameters);
    ComponentIndexType index1 = blueprint->AddComponent(transformComponentParameters);
    
    ParameterMapType metric2TransformConnectionParameters;
    metric2TransformConnectionParameters["NameOfInterface"] = ParameterValueType(1, "TransformedImageInterface");

    blueprint->AddConnection(index0, index1, metric2TransformConnectionParameters);

  }

  virtual void TearDown() {

  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;
};

TEST_F( OverlordTest, Create )
{
  overlord = Overlord::New();
  overlord->SetBlueprint(blueprint);
}

TEST_F(OverlordTest, Configure)
{
  overlord->Update();
}

} // namespace elx
