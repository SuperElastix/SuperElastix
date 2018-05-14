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

#include "selxSuperElastixFilterCustomComponents.h"

#include "selxNiftyregItkMultiStageComponent.h"

#include "selxItkToNiftiImageSourceComponent.h"
#include "selxNiftiToItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"
#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class NiftyregItkComponentTest : public ::testing::Test
{
public:

  typedef Blueprint::Pointer BlueprintPointer;
  typedef Blueprint::ParameterMapType ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef DataManager DataManagerType;

  /** register all example components */
  typedef TypeList< NiftyregItkMultiStageComponent<double, 2>,
	Niftyregf3dComponent< float >,
    ItkToNiftiImageSourceComponent< 2, float >,
    NiftiToItkImageSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkToNiftiImageSourceComponent< 3, float >,
    NiftiToItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 3, float >,
    NiftyregSplineToDisplacementFieldComponent< float>,
    DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
    NiftyregAladinComponent< float >> RegisterComponents;

  typedef SuperElastixFilterCustomComponents< RegisterComponents > SuperElastixFilterType;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New();
    dataManager = DataManagerType::New();
    logger = Logger::New();
    logger->AddStream("cout", std::cout);
    logger->SetLogLevel(LogLevel::TRC);
  }



  virtual void TearDown()
  {
    // Unregister all components after each test
    itk::ObjectFactoryBase::UnRegisterAllFactories();
    // Delete the SuperElastixFilter after each test
    superElastixFilter = nullptr;
  }

  BlueprintPointer blueprint;
  Logger::Pointer logger;
  SuperElastixFilterBase::Pointer superElastixFilter;
  DataManagerType::Pointer dataManager;
};


TEST_F(NiftyregItkComponentTest, AladinAndItk)
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod1", { { "NameOfClass", { "NiftyregAladinComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetComponent("RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("RegistrationMethod1", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } });

  blueprint->SetConnection("RegistrationMethod2", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });


  blueprint->Write(dataManager->GetOutputFile("NiftyregAffineAndBspline.dot"));

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("coneB2d64.mhd"));

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("NiftyregAladin_WBIR_Image.mhd"));

  //DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  //resultDisplacementWriter->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< Image2DType >("ResultImage"));
  //resultDisplacementWriter->SetInput(superElastixFilter->GetOutput< DisplacementImage2DType >("ResultDisplacementFieldSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
}


}
