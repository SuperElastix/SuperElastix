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
#include "selxDisplacementFieldComposerComponent.h"

#include "gtest/gtest.h"
#include "selxDataManager.h"

namespace selx {

class DisplacementFieldComposerComponentTest : public ::testing::Test {
public:

  typedef ItkDisplacementFieldComposerComponent< 2, float, float >::DisplacementFieldType DisplacementFieldType;
  typedef DisplacementFieldType::Pointer DisplacementFieldPointer;
  typedef itk::ImageFileWriter< DisplacementFieldType > DisplacementFieldWriterType;
  typedef DisplacementFieldWriterType::Pointer DisplacementFieldWriterPointer;

  typedef Blueprint::Pointer BlueprintPointer;

  typedef TypeList<
    ItkDisplacementFieldSourceComponent< 2, float >,
    ItkDisplacementFieldSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkImageSinkComponent< 2, float >,
    ItkDisplacementFieldComposerComponent< 2, float, float > > TestComponents;

  typedef SuperElastixFilterCustomComponents< TestComponents > SuperElastixFilterType;
  typedef SuperElastixFilterType::Pointer SuperElastixFilterPointer;

  DataManager::Pointer dataManager = DataManager::New();
};

TEST_F( DisplacementFieldComposerComponentTest, Compose )
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

  typedef itk::Vector<double, 2> VectorType;
  VectorType displacement_plus_11, displacement_minus_11;

  displacement_plus_11[0] = 1;
  displacement_plus_11[1] = 1;

  displacement_minus_11[0] = -1;
  displacement_minus_11[1] = -1;

  DisplacementFieldPointer warpingDisplacementField = DisplacementFieldType::New();
  warpingDisplacementField->SetRegions(DisplacementFieldType::SizeType({{3, 3}}));
  warpingDisplacementField->Allocate();

  warpingDisplacementField->SetPixel(displacementFieldIndex00, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex01, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex02, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex10, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex11, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex12, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex20, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex21, displacement_plus_11);
  warpingDisplacementField->SetPixel(displacementFieldIndex22, displacement_plus_11);

  DisplacementFieldPointer displacementField = DisplacementFieldType::New();
  displacementField->SetRegions(DisplacementFieldType::SizeType({{3, 3}}));
  displacementField->Allocate();

  displacementField->SetPixel(displacementFieldIndex00, displacement_minus_11);
  displacementField->SetPixel(displacementFieldIndex11, displacement_minus_11);

  Logger::Pointer logger = Logger::New();
  logger->AddStream( "cout", std::cout );
  logger->SetLogLevel( LogLevel::TRC );

  BlueprintPointer blueprint = Blueprint::New();
  using ParameterMapType = Blueprint::ParameterMapType;

  ParameterMapType displacementFieldSourceParameters;
  displacementFieldSourceParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldSourceComponent" };
  displacementFieldSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldSource", displacementFieldSourceParameters );

  ParameterMapType warpingDisplacementFieldSourceParameters;
  warpingDisplacementFieldSourceParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldSourceComponent" };
  warpingDisplacementFieldSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "WarpingDisplacementFieldSource", warpingDisplacementFieldSourceParameters );

  ParameterMapType displacementFieldComposerParameters;
  displacementFieldComposerParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldComposerComponent" };
  displacementFieldComposerParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldComposer", displacementFieldComposerParameters );

  ParameterMapType DisplacementFieldSinkParameters;
  DisplacementFieldSinkParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldSinkComponent" };
  DisplacementFieldSinkParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldSink", DisplacementFieldSinkParameters );

  ParameterMapType connection0;
  connection0[ "NameOfInterface" ] = { "itkDisplacementFieldInterface" };
  blueprint->SetConnection( "DisplacementFieldSource", "DisplacementFieldComposer", connection0 );

  ParameterMapType connection1;
  connection1[ "NameOfInterface" ] = { "itkWarpingDisplacementFieldInterface" };
  blueprint->SetConnection( "WarpingDisplacementFieldSource", "DisplacementFieldComposer", connection1 );

  ParameterMapType connection2;
  connection2[ "NameOfInterface" ] = { "itkDisplacementFieldInterface" };
  blueprint->SetConnection( "DisplacementFieldComposer", "DisplacementFieldSink", connection2 );

  SuperElastixFilterPointer superElastixFilter = SuperElastixFilterType::New();
  superElastixFilter->SetBlueprint(blueprint);
  superElastixFilter->SetLogger(logger);

  superElastixFilter->SetInput( "DisplacementFieldSource", displacementField );
  superElastixFilter->SetInput( "WarpingDisplacementFieldSource", warpingDisplacementField );

  auto composedDisplacementField = superElastixFilter->GetOutput< DisplacementFieldType >( "DisplacementFieldSink" );
  composedDisplacementField->Update();
  DisplacementFieldWriterPointer ImageWriter = DisplacementFieldWriterType::New();



  ImageWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldComposerComponentTest.Compose.DisplacementField.nii" ) );
  ImageWriter->SetInput( displacementField );
  ImageWriter->Update();

  ImageWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldComposerComponentTest.Compose.WarpingDisplacementField.nii" ) );
  ImageWriter->SetInput( warpingDisplacementField );
  ImageWriter->Update();

  ImageWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldComposerComponentTest.Compose.ComposedDisplacementField.nii" ) );
  ImageWriter->SetInput( composedDisplacementField );
  ImageWriter->Update();

  // Subset of pixels with forward transformation and zero inverse transformation
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex01)[0], 1);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex01)[1], 1);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex10)[0], 1);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex10)[1], 1);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex11)[0], 1);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex11)[1], 1);

  // Pixels with forward transformation and and identical inverse transformation
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex00)[0], 0);
  EXPECT_FLOAT_EQ(composedDisplacementField->GetPixel(displacementFieldIndex00)[1], 0);
}

} // namespace selx