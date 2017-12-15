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
#include "selxItkMeshSourceComponent.h"
#include "selxItkMeshSinkComponent.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "selxDisplacementFieldMeshWarperComponent.h"

#include "gtest/gtest.h"
#include "selxDataManager.h"

namespace selx {

class DisplacementFieldMeshWarperComponentTest : public ::testing::Test {
public:

  typedef ItkDisplacementFieldMeshWarperComponent< 2, float, float >::ItkDisplacementFieldType DisplacementFieldType;
  typedef DisplacementFieldType::Pointer DisplacementFieldPointer;
  typedef itk::ImageFileReader< DisplacementFieldType > DisplacementFieldReaderType;
  typedef DisplacementFieldReaderType::Pointer DisplacementFieldReaderPointer;
  typedef itk::ImageFileWriter< DisplacementFieldType > DisplacementFieldWriterType;
  typedef DisplacementFieldWriterType::Pointer DisplacementFieldWriterPointer;

  typedef itk::Mesh< float, 2 > MeshType;
  typedef MeshType::Pointer MeshPointer;
  typedef itk::MeshFileReader< MeshType > MeshReaderType;
  typedef MeshReaderType::Pointer MeshReaderPointer;
  typedef itk::MeshFileWriter< MeshType > MeshWriterType;
  typedef MeshWriterType::Pointer MeshWriterPointer;

  typedef Blueprint::Pointer BlueprintPointer;

  typedef TypeList<
    ItkDisplacementFieldSourceComponent< 2, float >,
    ItkDisplacementFieldSinkComponent< 2, float >,
    ItkMeshSourceComponent< 2, float >,
    ItkMeshSinkComponent< 2, float >,
    ItkDisplacementFieldMeshWarperComponent< 2, float, float > > TestComponents;

  typedef SuperElastixFilterCustomComponents< TestComponents > SuperElastixFilterType;
  typedef SuperElastixFilterType::Pointer SuperElastixFilterPointer;

  DataManager::Pointer dataManager = DataManager::New();
};

TEST_F( DisplacementFieldMeshWarperComponentTest, SinkAndSource )
{

  DisplacementFieldPointer displacementField = DisplacementFieldType::New();
  displacementField->SetRegions(DisplacementFieldType::Superclass::SizeType({{3, 3}}));
  displacementField->Allocate();

  DisplacementFieldType::Superclass::IndexType index00, index01, index02,
                                               index10, index11, index12,
                                               index20, index21, index22;

  index00[0] = 0; index00[1] = 0;
  index01[0] = 0; index01[1] = 1;
  index02[0] = 0; index02[1] = 2;
  index10[0] = 1; index10[1] = 0;
  index11[0] = 1; index11[1] = 1;
  index12[0] = 1; index12[1] = 2;
  index20[0] = 2; index20[1] = 0;
  index21[0] = 2; index21[1] = 1;
  index22[0] = 2; index22[1] = 2;

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

  displacementField->SetPixel(index00, displacement01);
  displacementField->SetPixel(index01, displacement10);
  displacementField->SetPixel(index02, displacement00);
  displacementField->SetPixel(index10, displacement10);
  displacementField->SetPixel(index11, displacement11);
  displacementField->SetPixel(index12, displacement00);
  displacementField->SetPixel(index20, displacement00);
  displacementField->SetPixel(index21, displacement00);
  displacementField->SetPixel(index22, displacementminus11);

  Logger::Pointer logger = Logger::New();
  logger->AddStream( "cout", std::cout );
  logger->SetLogLevel( LogLevel::TRC );

  BlueprintPointer blueprint = Blueprint::New();
  using ParameterMapType = Blueprint::ParameterMapType;

  ParameterMapType displacementFieldSourceParameters;
  displacementFieldSourceParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldSourceComponent" };
  displacementFieldSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldSource", displacementFieldSourceParameters );

  ParameterMapType meshSourceParameters;
  meshSourceParameters[ "NameOfClass" ]    = { "ItkMeshSourceComponent" };
  meshSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "MeshSource", meshSourceParameters );

  ParameterMapType meshSinkParameters;
  meshSinkParameters[ "NameOfClass" ]    = { "ItkMeshSinkComponent" };
  meshSinkParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "MeshSink", meshSinkParameters );

  ParameterMapType displacementFieldMeshWarperParameters;
  displacementFieldMeshWarperParameters[ "NameOfClass" ]    = { "ItkDisplacementFieldMeshWarperComponent" };
  displacementFieldMeshWarperParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldMeshWarper", displacementFieldMeshWarperParameters );

  ParameterMapType connection0;
  connection0[ "NameOfInterface" ] = { "itkMeshInterface" };
  blueprint->SetConnection( "MeshSource", "DisplacementFieldMeshWarper", connection0 );

  ParameterMapType connection1;
  connection0[ "NameOfInterface" ] = { "itkDisplacementFieldInterface" };
  blueprint->SetConnection( "DisplacementFieldSource", "DisplacementFieldMeshWarper", connection1 );

  ParameterMapType connection2;
  connection0[ "NameOfInterface" ] = { "itkMeshInterface" };
  blueprint->SetConnection( "DisplacementFieldMeshWarper", "MeshSink", connection2 );

  SuperElastixFilterPointer superElastixFilter = SuperElastixFilterType::New();
  superElastixFilter->SetBlueprint(blueprint);
  superElastixFilter->SetLogger(logger);

  MeshReaderPointer meshReader = MeshReaderType::New();
  meshReader->SetFileName( this->dataManager->GetInputFile( "2dSquare.vtk" ) );

  superElastixFilter->SetInput( "DisplacementFieldSource", displacementField );
  superElastixFilter->SetInput( "MeshSource", meshReader->GetOutput() );

  MeshWriterPointer meshWriter = MeshWriterType::New();
  meshWriter->SetFileName( this->dataManager->GetOutputFile( "DisplacementFieldMeshWarperComponentTest.SinkAndSource.WarpedMesh.vtk" ) );
  meshWriter->SetInput( superElastixFilter->GetOutput< MeshType >( "MeshSink" ) );
  meshWriter->Update();

  MeshPointer inputMesh = itkDynamicCastInDebugMode< MeshType * >( meshReader->GetOutput() );
  MeshPointer outputMesh = superElastixFilter->GetOutput< MeshType >( "MeshSink" );

  DisplacementFieldType::Superclass::IndexType index;
  displacementField->TransformPhysicalPointToIndex(inputMesh->GetPoint(0), index);
  EXPECT_EQ(inputMesh->GetPoint(0)+displacementField->GetPixel(index), outputMesh->GetPoint(0));
  displacementField->TransformPhysicalPointToIndex(inputMesh->GetPoint(1), index);
  EXPECT_EQ(inputMesh->GetPoint(1)+displacementField->GetPixel(index), outputMesh->GetPoint(1));
  displacementField->TransformPhysicalPointToIndex(inputMesh->GetPoint(2), index);
  EXPECT_EQ(inputMesh->GetPoint(2)+displacementField->GetPixel(index), outputMesh->GetPoint(2));
  displacementField->TransformPhysicalPointToIndex(inputMesh->GetPoint(3), index);
  EXPECT_EQ(inputMesh->GetPoint(3)+displacementField->GetPixel(index), outputMesh->GetPoint(3));
  displacementField->TransformPhysicalPointToIndex(inputMesh->GetPoint(0), index);
}

} // namespace selx