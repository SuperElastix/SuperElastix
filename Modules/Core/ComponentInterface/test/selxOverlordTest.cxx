#include "Overlord.h"

//#include "ComponentFactory.h"
#include "TransformComponent1.h"
#include "MetricComponent1.h"
#include "GDOptimizer3rdPartyComponent.h"
#include "GDOptimizer4thPartyComponent.h"
#include "SSDMetric3rdPartyComponent.h"
#include "SSDMetric4thPartyComponent.h"


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
    /** register all example components */
    ComponentFactory<TransformComponent1>::RegisterOneFactory();
    ComponentFactory<MetricComponent1>::RegisterOneFactory();
    ComponentFactory<GDOptimizer3rdPartyComponent>::RegisterOneFactory();
    ComponentFactory<GDOptimizer4thPartyComponent>::RegisterOneFactory();
    ComponentFactory<SSDMetric3rdPartyComponent>::RegisterOneFactory();
    ComponentFactory<SSDMetric4thPartyComponent>::RegisterOneFactory();

    /** make example blueprint configuration */
    blueprint = Blueprint::New();
    ParameterMapType metricComponentParameters;
    metricComponentParameters["NameOfClass"] = { "MetricComponent1" };

    ParameterMapType transformComponentParameters;
    transformComponentParameters["NameOfClass"] = { "TransformComponent1" };

    ComponentIndexType index0 = blueprint->AddComponent(metricComponentParameters);
    ComponentIndexType index1 = blueprint->AddComponent(transformComponentParameters);
    
    ParameterMapType metric2TransformConnectionParameters;
    metric2TransformConnectionParameters["NameOfInterface"] = { "TransformedImageInterface" };

    //TODO: check direction
    blueprint->AddConnection(index1, index0, metric2TransformConnectionParameters);

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;
};

TEST_F( OverlordTest, Create )
{
  EXPECT_NO_THROW( overlord = Overlord::New() );
  EXPECT_NO_THROW( overlord->SetBlueprint(blueprint) );
}

TEST_F(OverlordTest, Configure)
{
  overlord = Overlord::New();
  overlord->SetBlueprint(blueprint);
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
}

} // namespace selx
