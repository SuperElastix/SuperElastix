#include "Overlord.h"
//#include "elxElastixFilter.h"
#include "elxParameterObject.h"

#include "selxElastixComponent.h"
#include "selxItkImageFilterSink.h"
#include "selxItkImageSink.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx {

class ElastixComponentTest : public ::testing::Test {
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;
  typedef DataManager DataManagerType;
  
  virtual void SetUp() {
    /** register all example components */
    ComponentFactory<ElastixComponent<2, float>>::RegisterOneFactory();
    
    ComponentFactory<ItkImageFilterSinkComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSinkComponent<2, float>>::RegisterOneFactory();

    ComponentFactory<ItkImageSourceFixedComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<2, float>>::RegisterOneFactory();

    ComponentFactory<ItkImageSourceFixedComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<3, double>>::RegisterOneFactory();

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;
};

TEST_F(ElastixComponentTest, ImagesOnly)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ElastixComponent" };
  component0Parameters["RegistrationSettings"] = { "rigid" };
  ComponentIndexType index0 = blueprint->AddComponent("RegistrationMethod", component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index1 = blueprint->AddComponent("FixedImageSource", component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index2 = blueprint->AddComponent("MovingImageSource", component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  component3Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index3 = blueprint->AddComponent("ResultImageSink", component3Parameters);


  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);


  EXPECT_NO_THROW(overlord = Overlord::New());
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  
  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("BrainProtonDensitySliceBorder20.png"), dataManager->GetInputFile("BrainProtonDensitySliceR10X13Y17.png") };
  overlord->outputFileNames = { dataManager->GetOutputFile("ElastixComponentTest_BrainProtonDensity.mhd") };
  
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
  
}

} // namespace selx
