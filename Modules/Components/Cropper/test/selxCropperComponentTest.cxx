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
#include "selxItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "selxCropperComponent.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx {

class CropperComponentTest : public ::testing::Test {
public:

  typedef Blueprint::Pointer BlueprintPointer;
  typedef BlueprintImpl::ParameterMapType ParameterMapType;
  typedef BlueprintImpl::ParameterValueType ParameterValueType;
  typedef DataManager DataManagerType;

  /** Make a list of components to be registered for this test*/
  typedef TypeList<
    CropperComponent<2, float>,
    CropperComponent<3, float>,
    ItkImageSinkComponent<2, float>,
    ItkImageSinkComponent<3, float>,
    ItkImageSourceComponent<2, float>,
    ItkImageSourceComponent<2, unsigned char>,
    ItkImageSourceComponent<3, float>,
    ItkImageSourceComponent<3, unsigned char>
  > RegisterComponents;

  typedef CropperComponent<2, float> Cropper2DType;
  typedef CropperComponent<3, float> Cropper3DType;

  typedef itk::Image<float, 2> Image2DType;
  typedef itk::ImageFileReader<Image2DType> ImageReader2DType;
  typedef itk::ImageFileWriter<Image2DType> ImageWriter2DType;

  typedef itk::Image<float, 3> Image3DType;
  typedef itk::ImageFileReader<Image3DType> ImageReader3DType;
  typedef itk::ImageFileWriter<Image3DType> ImageWriter3DType;

  typedef itk::Image<unsigned char, 2> Mask2DType;
  typedef itk::ImageFileReader<Mask2DType> MaskReader2DType;

  typedef itk::Image<unsigned char, 3> Mask3DType;
  typedef itk::ImageFileReader<Mask3DType> MaskReader3DType;

  virtual void SetUp() {
    this->logger = Logger::New();
    this->logger->AddStream("cout", std::cout);
    this->logger->SetLogLevel(LogLevel::TRC);

    // Instantiate SuperElastixFilter before each test
    // Register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents<RegisterComponents>::New();
    superElastixFilter->SetLogger(logger);

    dataManager = DataManagerType::New();
  }


  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
    superElastixFilter = nullptr;
  }

  SuperElastixFilterCustomComponents<RegisterComponents>::Pointer superElastixFilter;
  DataManagerType::Pointer dataManager;
  Logger::Pointer logger;
};

TEST_F(CropperComponentTest, Instantiation) {
  auto cropperComponent2d = Cropper2DType("CropperComponent", this->logger->GetLoggerImpl());
  auto cropperComponent3d = Cropper3DType("CropperComponent", this->logger->GetLoggerImpl());
}

TEST_F(CropperComponentTest, Cropping2d) {
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "Image", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "Mask", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "unsigned char" } } } );
  blueprint->SetComponent( "CroppedImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "Cropper", { { "NameOfClass", { "ItkCropperComponent" } } });
  blueprint->SetConnection( "Image", "Cropper", {} );
  blueprint->SetConnection( "Mask", "Cropper", {} );
  blueprint->SetConnection( "Cropper", "CroppedImage", {} );

  superElastixFilter->SetBlueprint(blueprint);

  auto imageReader = ImageReader2DType::New();
  imageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  superElastixFilter->SetInput( "Image", imageReader->GetOutput() );

  auto maskReader = MaskReader2DType::New();
  maskReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20Mask.png" ) );
  superElastixFilter->SetInput( "Mask", maskReader->GetOutput() );

  auto croppedImageWriter = superElastixFilter->GetOutputFileWriter( "CroppedImage" );
  croppedImageWriter->SetFileName( dataManager->GetOutputFile( "CroppedImage.nii" ) );
  croppedImageWriter->SetInput( superElastixFilter->GetOutput( "CroppedImage" ) );
  croppedImageWriter->Update();

  this->logger->Log(LogLevel::INF, "Wrote " + dataManager->GetOutputFile( "CroppedImage.nii") + ".");
}

TEST_F(CropperComponentTest, Cropping3d) {
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "Image", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "Mask", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "unsigned char" } } } );
  blueprint->SetComponent( "CroppedImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "Cropper", { { "NameOfClass", { "ItkCropperComponent" } } });
  blueprint->SetConnection( "Image", "Cropper", {} );
  blueprint->SetConnection( "Mask", "Cropper", {} );
  blueprint->SetConnection( "Cropper", "CroppedImage", {} );

  superElastixFilter->SetBlueprint(blueprint);

  auto imageReader = ImageReader3DType::New();
  imageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  superElastixFilter->SetInput( "Image", imageReader->GetOutput() );

  auto maskReader = MaskReader3DType::New();
  maskReader->SetFileName( dataManager->GetInputFile( "sphereA3dMask.mhd" ) );
  superElastixFilter->SetInput( "Mask", maskReader->GetOutput() );

  auto croppedImageWriter = superElastixFilter->GetOutputFileWriter( "CroppedImage" );
  croppedImageWriter->SetFileName( dataManager->GetOutputFile( "CroppedsphereA3d.mhd.nii" ) );
  croppedImageWriter->SetInput( superElastixFilter->GetOutput( "CroppedImage" ) );
  croppedImageWriter->Update();

  this->logger->Log(LogLevel::INF, "Wrote " + dataManager->GetOutputFile( "CroppedsphereA3d.mhd.nii") + ".");
}

} // namespace selx