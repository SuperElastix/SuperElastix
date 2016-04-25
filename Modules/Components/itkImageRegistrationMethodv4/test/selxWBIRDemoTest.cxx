#include "Overlord.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageFilterSink.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"

#include "selxElastixComponent.h"
#include "selxItkImageFilterSink.h"
#include "selxItkImageSink.h"

#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace selx {

/** Temporary helper function to handle elastix deformation field output */
  template < int Dimensionality, typename PixelType> void CopyselxDeformationField(const std::string filename)
  {
    typedef itk::ImageFileReader<itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality>> ReaderType;
    typedef itk::ImageFileWriter<itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality>> WriterType;

    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName("deformationField.nii");
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(reader->GetOutput());
    writer->Update();
  };

/** A demo for our WBIR paper written as a Unit Test in the Google Test Framework */
class WBIRDemoTest : public ::testing::Test {
 
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ComponentIndexType     ComponentIndexType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;
  typedef DataManager DataManagerType;

  /** Fill SUPERelastix' component data base by registering various components */
  virtual void SetUp() {
    
    
    ComponentFactory<ItkImageFilterSinkComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<DisplacementFieldItkImageFilterSinkComponent<2, float>>::RegisterOneFactory(); 
    ComponentFactory<ItkImageSourceFixedComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageRegistrationMethodv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkMeanSquaresImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ElastixComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSinkComponent<2, float>>::RegisterOneFactory();

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  Overlord::Pointer overlord;

};

/** Experiment 2a: ITKv4 framework, stationary velocity field transform, ANTs neighborhood correlation metric */
TEST_F(WBIRDemoTest, itkv4_SVF_ANTSCC)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "DisplacementFieldItkImageFilterSinkComponent" };
  ComponentIndexType index4 = blueprint->AddComponent(component4Parameters);

  ParameterMapType component5Parameters;
  component5Parameters["NameOfClass"] = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  ComponentIndexType index5 = blueprint->AddComponent(component5Parameters);


  ParameterMapType connection1Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  ParameterMapType connection4Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->AddConnection(index0, index4, connection4Parameters);

  ParameterMapType connection5Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection(index5, index0, connection5Parameters);

  blueprint->WriteBlueprint("itkv4_SVF_ANTSCC.dot");

  EXPECT_NO_THROW(overlord = Overlord::New());

  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
  overlord->outputFileNames = { dataManager->GetOutputFile("itkv4_SVF_ANTSCC_Image.mhd"), dataManager->GetOutputFile("itkv4_SVF_ANTSCC_Displacement.mhd") };

  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
}

/** Experiment 2b: ITKv4 framework, stationary velocity field transform, mean squared differences metric */
TEST_F(WBIRDemoTest, itkv4_SVF_MSD)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "DisplacementFieldItkImageFilterSinkComponent" };
  ComponentIndexType index4 = blueprint->AddComponent(component4Parameters);

  ParameterMapType component5Parameters;
  component5Parameters["NameOfClass"] = { "ItkMeanSquaresImageToImageMetricv4Component" };
  ComponentIndexType index5 = blueprint->AddComponent(component5Parameters);


  ParameterMapType connection1Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  ParameterMapType connection4Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->AddConnection(index0, index4, connection4Parameters);

  ParameterMapType connection5Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection(index5, index0, connection5Parameters);

  blueprint->WriteBlueprint("itkv4_SVF_MSD.dot");

  EXPECT_NO_THROW(overlord = Overlord::New());

  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
  overlord->outputFileNames = { dataManager->GetOutputFile("itkv4_SVF_MSD_Image.mhd"), dataManager->GetOutputFile("itkv4_SVF_MSD_Displacement.mhd") };
  
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());
}

/** Experiment 1a: elastix framework, B-spline transform, normalized correlation metric */
TEST_F(WBIRDemoTest, elastix_BS_NCC)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ElastixComponent" };
  //component0Parameters["RegistrationPreset"] = { "rigid" };
  component0Parameters["Transform"] = { "BSplineTransform" }; 
  component0Parameters["Metric"] = { "AdvancedNormalizedCorrelation" };
  
  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType connection1Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  blueprint->WriteBlueprint("elastix_BS_NCC.dot");

  EXPECT_NO_THROW(overlord = Overlord::New());
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));

  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
  overlord->outputFileNames = { dataManager->GetOutputFile("elastix_BS_NCC_Image.mhd"), dataManager->GetOutputFile("elastix_BS_NCC_Displacement.mhd") };

  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());

  CopyselxDeformationField<2, float>(dataManager->GetOutputFile("elastix_BS_NCC_Displacement.mhd"));
}

/** Experiment 1b: elastix framework, B-spline transform, mean squared differences metric */
TEST_F(WBIRDemoTest, elastix_BS_MSD)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ElastixComponent" };
  //component0Parameters["RegistrationPreset"] = { "rigid" };
  component0Parameters["Transform"] = { "BSplineTransform" };
  component0Parameters["Metric"] = { "AdvancedMeanSquares" };

  ComponentIndexType index0 = blueprint->AddComponent(component0Parameters);


  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  ComponentIndexType index1 = blueprint->AddComponent(component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  ComponentIndexType index2 = blueprint->AddComponent(component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  ComponentIndexType index3 = blueprint->AddComponent(component3Parameters);

  ParameterMapType connection1Parameters;
  connection1Parameters["NameOfInterface"] = { "itkImageSourceFixedInterface" };
  blueprint->AddConnection(index1, index0, connection1Parameters);

  ParameterMapType connection2Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection2Parameters["NameOfInterface"] = { "itkImageSourceMovingInterface" };
  blueprint->AddConnection(index2, index0, connection2Parameters);

  ParameterMapType connection3Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
  blueprint->AddConnection(index0, index3, connection3Parameters);

  blueprint->WriteBlueprint("elastix_BS_MSD.dot");

  EXPECT_NO_THROW(overlord = Overlord::New());
  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));

  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  DataManagerType::Pointer dataManager = DataManagerType::New();
  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
  overlord->outputFileNames = { dataManager->GetOutputFile("elastix_BS_MSD_Image.mhd"), dataManager->GetOutputFile("elastix_BS_MSD_Displacement.mhd") };

  bool allUniqueComponents;
  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
  EXPECT_TRUE(allUniqueComponents);
  EXPECT_NO_THROW(overlord->Execute());

  CopyselxDeformationField<2, float>(dataManager->GetOutputFile("elastix_BS_MSD_Displacement.mhd"));
}

} // namespace selx

