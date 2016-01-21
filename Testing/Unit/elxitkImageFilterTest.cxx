#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageSink.h"
#include "selxItkImageSource.h"
#include "Overlord.h"

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
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;

  virtual void SetUp() {


    /** register all components used for this test */
    // For testing the itkfilter pipeline
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3,double>>::RegisterOneFactory(); 
    ComponentFactory<ItkImageSinkComponent>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceComponent>::RegisterOneFactory();
    // For testing templated components
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();


    /** make example blueprint configuration */
    blueprint = Blueprint::New();

    /** the 2 itkImageFilter Components are ItkSmoothingRecursiveGaussianImageFilterComponent*/ 
    ParameterMapType componentParameters;
    componentParameters["NameOfClass"] = ParameterValueType(1, "ItkSmoothingRecursiveGaussianImageFilterComponent");
    // The parameters over which the Component is templated are criteria for the ComponentSelector too.
    componentParameters["Dimensionality"] = ParameterValueType(1, "3");
    componentParameters["PixelType"] = ParameterValueType(1, "double");

    // Setting of the component are considered as criteria too. If the components can interpret the parameters, it's all good. 
    componentParameters["Sigma"] = ParameterValueType(1, "2.5");

    ComponentIndexType index0 = blueprint->AddComponent(componentParameters);
    ComponentIndexType index1 = blueprint->AddComponent(componentParameters);
    
    // TODO: For now, the connections to make are explicitly indicated by the Interface name. 
    // Design consideration: connections might be indicated by higher concepts ( MetricCostConnection: value and/or derivative? DefaultPipeline? ) 
    ParameterMapType connectionParameters;
    connectionParameters["NameOfInterface"] = ParameterValueType(1, "itkImageSourceInterface");

    //TODO: check direction
    blueprint->AddConnection(index0, index1, connectionParameters);

    /** Add a description of the SourceComponent*/
    ParameterMapType sourceComponentParameters;
    sourceComponentParameters["NameOfClass"] = ParameterValueType(1, "ItkImageSourceComponent");
    ComponentIndexType sourceIndex = blueprint->AddComponent(sourceComponentParameters);

    /** Add a description of the SinkComponent*/
    ParameterMapType sinkComponentParameters;
    sinkComponentParameters["NameOfClass"] = ParameterValueType(1, "ItkImageSinkComponent");
    ComponentIndexType sinkIndex = blueprint->AddComponent(sinkComponentParameters);
    
    /** Connect Sink and Source to the itkImageFilter Components*/
    blueprint->AddConnection(sourceIndex,index0, connectionParameters); // 
    blueprint->AddConnection(index1, sinkIndex, connectionParameters);
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

} // namespace elx
