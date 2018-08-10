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
#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxItkDisplacementFieldSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "selxItkImageSinkComponent.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "selxDisplacementFieldImageWarperComponent.h"

#include "gtest/gtest.h"
#include "selxDataManager.h"

namespace selx {

class DisplacementFieldImageWarperComponentTest : public ::testing::Test {
public:

  typedef ItkDisplacementFieldImageWarperComponent< 2, float, float >::DisplacementFieldType DisplacementFieldType;
  typedef DisplacementFieldType::Pointer DisplacementFieldPointer;
  typedef itk::ImageFileReader< DisplacementFieldType > DisplacementFieldReaderType;
  typedef DisplacementFieldReaderType::Pointer DisplacementFieldReaderPointer;
  typedef itk::ImageFileWriter< DisplacementFieldType > DisplacementFieldWriterType;
  typedef DisplacementFieldWriterType::Pointer DisplacementFieldWriterPointer;

  typedef itk::Image< float, 2 > ImageType;
  typedef ImageType::Pointer ImagePointer;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer ImageReaderPointer;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;
  typedef ImageWriterType::Pointer ImageWriterPointer;

  typedef Blueprint::Pointer BlueprintPointer;

  typedef TypeList<
    ItkDisplacementFieldSourceComponent< 2, float >,
    ItkDisplacementFieldSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkImageSinkComponent< 2, float >,
    ItkDisplacementFieldImageWarperComponent< 2, float, float > > TestComponents;

  typedef SuperElastixFilterCustomComponents< TestComponents > SuperElastixFilterType;
  typedef SuperElastixFilterType::Pointer SuperElastixFilterPointer;

  DataManager::Pointer dataManager = DataManager::New();
};

TEST_F( DisplacementFieldImageWarperComponentTest, SinkAndSource )
{

  DisplacementFieldType::Superclass::IndexType displacementFieldIndex00, displacementFieldIndex01, displacementFieldIndex02,
      displacementFieldIndex10, displacementFieldIndex11, displacementFieldIndex12,
      displacementFieldIndex20, displacementFieldIndex21, displacementFieldIndex22;

  displacementFieldIndex00[0] = 0; displacementFieldIndex00[1] = 0;
  displacementFieldIndex01[0] = 0; displacementFieldIndex01[1] = 1;
  displacementFieldIndex02[0] = 0; displacementFieldIndex02[1] = 2;
  displacementFieldIndex10[0] = 1; displacementFieldIndex10[1] = 0;
  displacementFieldIndex11[0] = 1; displacementFieldIndex11[1] = 1;
  displacementFieldIndex12[0] = 1; displacementFieldIndex12[1] = 2;
  displacementFieldIndex20[0] = 2; displacementFieldIndex20[1] = 0;
  displacementFieldIndex21[0] = 2; displacementFieldIndex21[1] = 1;
  displacementFieldIndex22[0] = 2; displacementFieldIndex22[1] = 2;

  typedef itk::Vector<float, 2> VectorType;
  VectorType displacement00, displacement01, displacement10, displacement11, displacementminus11;
  displacement00[0] = 0.;
  displacement00[1] = 0.;

  displacement01[0] = 0.;
  displacement01[1] = 1.;

  displacement10[0] = 1.;
  displacement10[1] = 0.;

  displacement11[0] = 1.;
  displacement11[1] = 1.;

  displacementminus11[0] = -1.;
  displacementminus11[1] = -1.;

  DisplacementFieldPointer displacementField = DisplacementFieldType::New();
  displacementField->SetRegions(DisplacementFieldType::SizeType({{3, 3}}));
  displacementField->Allocate();
  
  displacementField->SetPixel(displacementFieldIndex00, displacement01);
  displacementField->SetPixel(displacementFieldIndex01, displacement01);
  displacementField->SetPixel(displacementFieldIndex02, displacement01);
  displacementField->SetPixel(displacementFieldIndex10, displacement01);
  displacementField->SetPixel(displacementFieldIndex11, displacement01);
  displacementField->SetPixel(displacementFieldIndex12, displacement01);
  displacementField->SetPixel(displacementFieldIndex20, displacement01);
  displacementField->SetPixel(displacementFieldIndex21, displacement01);
  displacementField->SetPixel(displacementFieldIndex22, displacement01);

  ImageType::IndexType imageIndex00, imageIndex01, imageIndex02,
      imageIndex10, imageIndex11, imageIndex12,
      imageIndex20, imageIndex21, imageIndex22;

  imageIndex00[0] = 0; imageIndex00[1] = 0;
  imageIndex01[0] = 0; imageIndex01[1] = 1;
  imageIndex02[0] = 0; imageIndex02[1] = 2;
  imageIndex10[0] = 1; imageIndex10[1] = 0;
  imageIndex11[0] = 1; imageIndex11[1] = 1;
  imageIndex12[0] = 1; imageIndex12[1] = 2;
  imageIndex20[0] = 2; imageIndex20[1] = 0;
  imageIndex21[0] = 2; imageIndex21[1] = 1;
  imageIndex22[0] = 2; imageIndex22[1] = 2;

  ImagePointer image = ImageType::New();
  ImageType::SizeType size = {3, 3};
  image->SetRegions(size);
  image->Allocate();
  
  image->SetPixel(imageIndex00, 0.f);
  image->SetPixel(imageIndex01, 1.f);
  image->SetPixel(imageIndex02, 2.f);
  image->SetPixel(imageIndex10, 3.f);
  image->SetPixel(imageIndex11, 4.f);
  image->SetPixel(imageIndex12, 5.f);
  image->SetPixel(imageIndex20, 6.f);
  image->SetPixel(imageIndex21, 7.f);
  image->SetPixel(imageIndex22, 8.f);

  ImagePointer resultImage = ImageType::New();
  resultImage->SetRegions(size);
  resultImage->Allocate();

  resultImage->SetPixel(imageIndex00, 1.f);

  Logger::Pointer logger = Logger::New();
  logger->AddStream( "cout", std::cout );
  logger->SetLogLevel( LogLevel::TRC );

  BlueprintPointer blueprint = Blueprint::New();
  using ParameterMapType = Blueprint::ParameterMapType;

  ParameterMapType displacementFieldSourceParameters;
  displacementFieldSourceParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldSourceComponent" };
  displacementFieldSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldSource", displacementFieldSourceParameters );

  ParameterMapType ImageSourceParameters;
  ImageSourceParameters[ "NameOfClass" ]    = { "ItkImageSourceComponent" };
  ImageSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "ImageSource", ImageSourceParameters );

  ParameterMapType ImageSinkParameters;
  ImageSinkParameters[ "NameOfClass" ]    = { "ItkImageSinkComponent" };
  ImageSinkParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "ImageSink", ImageSinkParameters );

  ParameterMapType displacementFieldImageWarperParameters;
  displacementFieldImageWarperParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldImageWarperComponent" };
  displacementFieldImageWarperParameters[ "Dimensionality" ] = { "2" };
  displacementFieldSourceParameters[ "Interpolator" ] = { "NearestNeighbor" };
  blueprint->SetComponent( "DisplacementFieldImageWarper", displacementFieldImageWarperParameters );

  ParameterMapType connection0;
  connection0[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "ImageSource", "DisplacementFieldImageWarper", connection0 );

  ParameterMapType connection1;
  connection0[ "NameOfInterface" ] = { "itkDisplacementFieldInterface" };
  blueprint->SetConnection( "DisplacementFieldSource", "DisplacementFieldImageWarper", connection1 );

  ParameterMapType connection2;
  connection0[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "DisplacementFieldImageWarper", "ImageSink", connection2 );

  SuperElastixFilterPointer superElastixFilter = SuperElastixFilterType::New();
  superElastixFilter->SetBlueprint(blueprint);
  superElastixFilter->SetLogger(logger);

  superElastixFilter->SetInput( "DisplacementFieldSource", displacementField );
  superElastixFilter->SetInput( "ImageSource", image );

  ImageWriterPointer ImageWriter = ImageWriterType::New();
  ImageWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldImageWarperComponentTest.SinkAndSource.WarpedImage.nii" ) );
  ImageWriter->SetInput( superElastixFilter->GetOutput< ImageType >( "ImageSink" ) );
  ImageWriter->Update();

  ImageWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldImageWarperComponentTest.SinkAndSource.OriginalImage.nii" ) );
  ImageWriter->SetInput( image );
  ImageWriter->Update();
  
  ImagePointer outputImage = superElastixFilter->GetOutput< ImageType >( "ImageSink" );

  EXPECT_FLOAT_EQ(image->GetPixel(imageIndex01), outputImage->GetPixel(imageIndex00));
  
}

} // namespace selx