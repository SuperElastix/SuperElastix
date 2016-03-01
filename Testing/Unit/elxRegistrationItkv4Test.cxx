#include "Overlord.h"

//#include "ComponentFactory.h"
#include "TransformComponent1.h"
#include "MetricComponent1.h"
#include "GDOptimizer3rdPartyComponent.h"
#include "GDOptimizer4thPartyComponent.h"
#include "SSDMetric3rdPartyComponent.h"
#include "SSDMetric4thPartyComponent.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageFilterSink.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"


#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

namespace selx {

class RegistrationItkv4Test : public ::testing::Test {
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
    ComponentFactory<TransformComponent1>::RegisterOneFactory();
    ComponentFactory<MetricComponent1>::RegisterOneFactory();
    ComponentFactory<GDOptimizer3rdPartyComponent>::RegisterOneFactory();
    ComponentFactory<GDOptimizer4thPartyComponent>::RegisterOneFactory();
    ComponentFactory<SSDMetric3rdPartyComponent>::RegisterOneFactory();
    ComponentFactory<SSDMetric4thPartyComponent>::RegisterOneFactory();

    
    ComponentFactory<ItkImageFilterSinkComponent<3,double>>::RegisterOneFactory();
    ComponentFactory<ItkImageFilterSinkComponent<2, float>>::RegisterOneFactory();

    ComponentFactory<DisplacementFieldItkImageFilterSinkComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<DisplacementFieldItkImageFilterSinkComponent<2, float>>::RegisterOneFactory();
    
    //ComponentFactory<ItkImageSourceComponent>::RegisterOneFactory();

    ComponentFactory<ItkImageSourceFixedComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<2, float>>::RegisterOneFactory();

    ComponentFactory<ItkImageSourceFixedComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<3, double>>::RegisterOneFactory();


    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();

    ComponentFactory<ItkImageRegistrationMethodv4Component<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageRegistrationMethodv4Component<2, float>>::RegisterOneFactory();

    ComponentFactory<ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkMeanSquaresImageToImageMetricv4Component<3, double>>::RegisterOneFactory();

    ComponentFactory<ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkMeanSquaresImageToImageMetricv4Component<2, float>>::RegisterOneFactory();

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;


};

TEST_F(RegistrationItkv4Test, ImagesOnly)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  //component3Parameters["Dimensionality"] = { "3" }; // should be derived from the outputs
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);


  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);


  EXPECT_NO_THROW(overlord = Overlord::New());
  overlord->inputFileNames = { "source3dimage0.mhd", "source3dimage1.mhd" };
  overlord->outputFileNames = { "sink3dimage0.mhd" };
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
  
}

TEST_F(RegistrationItkv4Test, WithANTSCCMetric)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);
  
  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  component3Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component4Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index4 = blueprint->AddComponent(component4Parameters);


  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  ParameterMapType connection4Parameters;
  connection4Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection(index4, index0, connection4Parameters);

  EXPECT_NO_THROW(overlord = Overlord::New());
  overlord->inputFileNames = { "source3dimage0.mhd", "source3dimage1.mhd" };
  overlord->outputFileNames = { "sink3dimage0.mhd" };
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
  //overlord->Execute();
}
TEST_F(RegistrationItkv4Test, WithMeanSquaresMetric)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  component3Parameters["Dimensionality"] = { "3" }; // should be derived from the inputs
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "ItkMeanSquaresImageToImageMetricv4Component" };
  component3Parameters["Dimensionality"] = { "4" }; // should be derived from the inputs
  ComponentIndexType index4 = blueprint->AddComponent(component4Parameters);


  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  ParameterMapType connection4Parameters;
  connection4Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection(index4, index0, connection4Parameters);

  EXPECT_NO_THROW(overlord = Overlord::New());
  overlord->inputFileNames = { "source3dimage0.mhd", "source3dimage1.mhd" };
  overlord->outputFileNames = { "sink3dimage0.mhd" };
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
  //overlord->Execute();
}

TEST_F(RegistrationItkv4Test, DisplacementField2D)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  component0Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  component3Parameters["Dimensionality"] = { "2" }; // should be derived from the outputs
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "DisplacementFieldItkImageFilterSinkComponent" };
  component4Parameters["Dimensionality"] = { "2" }; // should be derived from the outputs
  ComponentIndexType index4 = blueprint->AddComponent(component4Parameters);

  ParameterMapType component5Parameters;
  component5Parameters["NameOfClass"] = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component5Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  ComponentIndexType index5 = blueprint->AddComponent(component5Parameters);


  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  ParameterMapType connection4Parameters;
  connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->AddConnection(index0, index4, connection4Parameters);

  ParameterMapType connection5Parameters;
  connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection(index5, index0, connection5Parameters);

  EXPECT_NO_THROW(overlord = Overlord::New());

  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("BrainProtonDensitySliceBorder20.png"), dataManager->GetInputFile("BrainProtonDensitySliceR10X13Y17.png") };
  overlord->outputFileNames = { dataManager->GetOutputFile("RegistrationItkv4Test_BrainProtonDensity.mhd"), dataManager->GetOutputFile("RegistrationItkv4Test_Displacement_BrainProtonDensity.mhd") };

  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  //EXPECT_NO_THROW(overlord->Execute());
  overlord->Execute();
}
} // namespace elx

