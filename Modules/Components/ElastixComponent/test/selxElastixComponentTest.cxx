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

#include "selxSuperElastixFilter.h"

#include "elxParameterObject.h"

#include "selxElastixComponent.h"
#include "selxItkImageSink.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx {

class ElastixComponentTest : public ::testing::Test {
public:
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;
  typedef DataManager DataManagerType;
  typedef SuperElastixFilter<bool>          SuperElastixFilterType;

  typedef itk::Image<float, 2> Image2DType;
  typedef itk::ImageFileReader<Image2DType> ImageReader2DType;
  typedef itk::ImageFileWriter<Image2DType> ImageWriter2DType;

  virtual void SetUp() {
    /** register all example components */
    ComponentFactory<ElastixComponent<2, float>>::RegisterOneFactory();
    
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
  blueprint->AddComponent("RegistrationMethod", component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  component1Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  blueprint->AddComponent("FixedImageSource", component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  component2Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  blueprint->AddComponent("MovingImageSource", component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  component3Parameters["Dimensionality"] = { "2" }; // should be derived from the inputs
  blueprint->AddComponent("ResultImageSink", component3Parameters);


  ParameterMapType connection1Parameters;
  //connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);

  ParameterMapType connection2Parameters;
  //connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);

  ParameterMapType connection3Parameters;
  //connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);

  // Instantiate SuperElastix
  SuperElastixFilterType::Pointer superElastixFilter;
  EXPECT_NO_THROW(superElastixFilter = SuperElastixFilterType::New());

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("BrainProtonDensitySliceBorder20.png"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("BrainProtonDensitySliceR10X13Y17.png"));

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("ElastixComponentTest_BrainProtonDensity.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImageSource", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImageSource", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput<Image2DType>("ResultImageSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
  
}

} // namespace selx
