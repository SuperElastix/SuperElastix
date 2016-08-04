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
  //typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
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

    blueprint->AddComponent("Metric", metricComponentParameters);
    blueprint->AddComponent("Transform", transformComponentParameters);
    
    ParameterMapType metric2TransformConnectionParameters;
    metric2TransformConnectionParameters["NameOfInterface"] = { "TransformedImageInterface" };

    //TODO: check direction
    blueprint->AddConnection("Transform","Metric", metric2TransformConnectionParameters);

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  //std::unique_ptr<Blueprint> blueprint(new Blueprint());
  BlueprintPointerType blueprint;
  //Overlord::Pointer overlord;
};

TEST_F( OverlordTest, Create )
{
  std::unique_ptr<Overlord> overlord(new Overlord());
  EXPECT_NO_THROW( overlord->SetBlueprint(blueprint) );
}

TEST_F(OverlordTest, Configure)
{
  std::unique_ptr<Overlord> overlord(new Overlord());
  overlord->SetBlueprint(blueprint);
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
}

} // namespace selx
