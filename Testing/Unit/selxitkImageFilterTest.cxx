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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageSink.h"
#include "selxItkImageSource.h"
#include "Overlord.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

/** this test uses the following blueprint setup

 [SourceComponent] - [itkImageFilter] - [itkImageFilter] - [SinkComponent]

 - All the nodes are identified by their Class names
 - All Connections are identified by their Interface names

 The overlord finds the Source and Sink Components and connects these to it's external pipeline (internal reader and writer filters, currently).
*/

namespace selx {

  class itkImageFilterTest : public ::testing::Test {
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;
  typedef DataManager DataManagerType;

  virtual void SetUp() {


    /** register all components used for this test */
    // For testing the itkfilter pipeline
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSinkComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceComponent<3, double>>::RegisterOneFactory();
    // For testing templated components
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();


    /** make example blueprint configuration */
    blueprint = Blueprint::New();

    /** the 2 itkImageFilter Components are ItkSmoothingRecursiveGaussianImageFilterComponent*/
    ParameterMapType componentParameters;
    componentParameters["NameOfClass"] = { "ItkSmoothingRecursiveGaussianImageFilterComponent" };
    // The parameters over which the Component is templated are criteria for the ComponentSelector too.
    componentParameters["Dimensionality"] = { "3" }; // template parameters
    componentParameters["PixelType"] = { "double" };

    // Setting of the component are considered as criteria too. If the components can interpret the parameters, it's all good. 
    componentParameters["Sigma"] = { "2.5" };

    blueprint->AddComponent("FistStageFilter", componentParameters);
    blueprint->AddComponent("SecondStageFilter", componentParameters);

    // TODO: For now, the connections to make are explicitly indicated by the Interface name. 
    // Design consideration: connections might be indicated by higher concepts ( MetricCostConnection: value and/or derivative? DefaultPipeline? ) 
    ParameterMapType connectionParameters;
    //connectionParameters["NameOfInterface"] = { "itkImageSourceInterface" };

    //TODO: check direction
    blueprint->AddConnection("FistStageFilter", "SecondStageFilter", connectionParameters);

    /** Add a description of the SourceComponent*/
    ParameterMapType sourceComponentParameters;
    sourceComponentParameters["NameOfClass"] = { "ItkImageSourceComponent" };
    blueprint->AddComponent("Source", sourceComponentParameters);

    /** Add a description of the SinkComponent*/
    ParameterMapType sinkComponentParameters;
    sinkComponentParameters["NameOfClass"] = { "ItkImageSinkComponent" };
    blueprint->AddComponent("Sink", sinkComponentParameters);
    
    /** Connect Sink and Source to the itkImageFilter Components*/
    blueprint->AddConnection("Source", "FistStageFilter", connectionParameters); // 
    blueprint->AddConnection("SecondStageFilter", "Sink", connectionParameters);
  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;
};

  TEST_F(itkImageFilterTest, Run)
{
  overlord = Overlord::New();
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("sphereA3d.mhd") };
  overlord->outputFileNames = { dataManager->GetOutputFile("itkImageFilterTest.mhd") };
  overlord->SetBlueprint(blueprint);
  bool allUniqueComponents;
  
  // Read the blueprint and try to realize all components
  // If for any node no components could be selected an exception is thrown.
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  // If for any node multiple components are selected, allUniqueComponents is false.
  EXPECT_TRUE(allUniqueComponents);

  // If Configuration was successful, the graph (pipeline) can be executed.
  EXPECT_NO_THROW(overlord->Execute());
}

} // namespace selx
