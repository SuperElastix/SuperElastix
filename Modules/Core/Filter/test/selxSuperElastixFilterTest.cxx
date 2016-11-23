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
#include "itkSharedPointerDataObjectDecorator.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageSink.h"
#include "selxItkImageSource.h"
#include "selxItkMeshSink.h"
#include "selxItkMeshSource.h"
#include "selxRegistrationController.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDisplacementFieldTransform.h"
#include "itkComposeDisplacementFieldsImageFilter.h"

#include "itkCompositeTransform.h"
#include "itkVector.h"
#include "itkPoint.h"
#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

#include "selxDefaultComponents.h"
#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class SuperElastixFilterTest : public ::testing::Test
{
public:

  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  typedef itk::Image< double, 3 >             Image3DType;
  typedef itk::ImageFileReader< Image3DType > ImageReader3DType;
  typedef itk::ImageFileWriter< Image3DType > ImageWriter3DType;

  /** Fill SUPERelastix' component data base by registering various components */
  typedef TypeList<
    ItkImageSinkComponent< 3, double >,
    ItkImageSourceComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, double >,
    ItkMeshSinkComponent< 2, float >,
    ItkMeshSourceComponent< 2, float > > RegisterComponents;

  typedef std::shared_ptr< Blueprint >                  BlueprintPointer;

  typedef SuperElastixFilter< RegisterComponents >      SuperElastixFilterType;
  typedef SuperElastixFilterType::Pointer               SuperElastixFilterPointer;
  typedef SuperElastixFilterType::BlueprintType         SuperElastixFilterBlueprintType;
  typedef SuperElastixFilterType::BlueprintPointer      SuperElastixFilterBlueprintPointer;

  typedef itk::Mesh< float, 2 >           MeshType;
  typedef itk::MeshFileReader< MeshType > MeshReaderType;
  typedef itk::MeshFileWriter< MeshType > MeshWriterType;

  typedef DataManager DataManagerType;
  virtual void SetUp()
  {
  }


  virtual void TearDown()
  {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }
};

TEST_F( SuperElastixFilterTest, ImageOnly )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
  ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

  imageReader3D->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageWriter3D->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_sphereA3d.mhd" ) );

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } } );
  blueprint->SetComponent( "ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } } );
  blueprint->SetComponent( "OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } } );
  blueprint->SetConnection( "InputImage", "ImageFilter", { {} } ); //
  blueprint->SetConnection( "ImageFilter", "OutputImage", { {} } );

  SuperElastixFilterType::Pointer superElastix;
  EXPECT_NO_THROW( superElastix = SuperElastixFilterType::New() );

  SuperElastixFilterBlueprintPointer superElastixBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixBlueprint->Set( blueprint );
  superElastix->SetBlueprint( superElastixBlueprint );

  superElastix->SetInput( "InputImage", imageReader3D->GetOutput() );
  imageWriter3D->SetInput( superElastix->GetOutput< Image3DType >( "OutputImage" ) );

  imageWriter3D->Update();
}
TEST_F( SuperElastixFilterTest, ImageAndMesh )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
  ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

  imageReader3D->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageWriter3D->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_sphereA3d.mhd" ) );

  MeshReaderType::Pointer meshReader = MeshReaderType::New();
  MeshWriterType::Pointer meshWriter = MeshWriterType::New();

  meshReader->SetFileName( dataManager->GetInputFile( "2dSquare.vtk" ) );
  meshWriter->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_2dSquare.vtk" ) );

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } } );
  blueprint->SetComponent( "ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } } );
  blueprint->SetComponent( "OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } } );
  blueprint->SetConnection( "InputImage", "ImageFilter", Blueprint::ParameterMapType() ); //
  blueprint->SetConnection( "ImageFilter", "OutputImage", Blueprint::ParameterMapType() );

  blueprint->SetComponent( "InputMesh", { { "NameOfClass", { "ItkMeshSourceComponent" } } } );
  blueprint->SetComponent( "OutputMesh", { { "NameOfClass", { "ItkMeshSinkComponent" } } } );
  blueprint->SetConnection( "InputMesh", "OutputMesh", Blueprint::ParameterMapType() );

  SuperElastixFilterType::Pointer superElastix;
  EXPECT_NO_THROW( superElastix = SuperElastixFilterType::New() );

  SuperElastixFilterBlueprintPointer superElastixFilterBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixFilterBlueprint->Set( blueprint );
  superElastix->SetBlueprint( superElastixFilterBlueprint );

  superElastix->SetInput( "InputMesh", meshReader->GetOutput() );
  meshWriter->SetInput( superElastix->GetOutput< MeshType >( "OutputMesh" ) );

  superElastix->SetInput( "InputImage", imageReader3D->GetOutput() );
  imageWriter3D->SetInput( superElastix->GetOutput< Image3DType >( "OutputImage" ) );

  EXPECT_NO_THROW( meshWriter->Update() );
  EXPECT_NO_THROW( imageWriter3D->Update() );
}
TEST_F( SuperElastixFilterTest, TooManyInputs )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();
  ImageReader3DType::Pointer imageReader3D_B = ImageReader3DType::New();

  imageReader3D_A->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageReader3D_B->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  std::cout << "Create blueprint " << std::endl;
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } } } );

  SuperElastixFilterType::Pointer superElastix;
  EXPECT_NO_THROW( superElastix = SuperElastixFilterType::New() );

  SuperElastixFilterBlueprintPointer superElastixFilterBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixFilterBlueprint->Set( blueprint );
  superElastix->SetBlueprint( superElastixFilterBlueprint );

  std::cout << "Set inputs " << std::endl;
  superElastix->SetInput( "Source_A", imageReader3D_A->GetOutput() );
  superElastix->SetInput( "Source_B", imageReader3D_B->GetOutput() );

  std::cout << "Update " << std::endl;
  EXPECT_THROW( superElastix->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManySources )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();

  imageReader3D_A->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } } } );
  blueprint->SetComponent( "Source_B", { { "NameOfClass", { "ItkImageSourceComponent" } } } );

  SuperElastixFilterType::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  SuperElastixFilterBlueprintPointer superElastixFilterBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  superElastixFilter->SetInput( "Source_A", imageReader3D_A->GetOutput() );

  EXPECT_THROW( superElastixFilter->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManyOutputs )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
  ImageWriter3DType::Pointer imageWriter3D_B = ImageWriter3DType::New();
  imageWriter3D_A->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );
  imageWriter3D_B->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } } } );

  SuperElastixFilterPointer superElastixFilter = SuperElastixFilterType::New();
  SuperElastixFilterBlueprintPointer superElastixFilterBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixFilterBlueprint->Set( blueprint );
  superElastixFilter->SetBlueprint( superElastixFilterBlueprint );

  imageWriter3D_A->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_A" ) );
  imageWriter3D_B->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_B" ) );

  EXPECT_THROW( imageWriter3D_B->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManySinks )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
  imageWriter3D_A->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );

  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } } } );
  blueprint->SetComponent( "Sink_B", { { "NameOfClass", { "ItkImageSinkComponent" } } } );

  SuperElastixFilterType::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  SuperElastixFilterBlueprintPointer superElastixFilterBlueprint = SuperElastixFilterBlueprintType::New();
  superElastixFilterBlueprint->Set( blueprint );
  superElastixFilter->SetBlueprint( superElastixFilterBlueprint );

  imageWriter3D_A->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_A" ) );

  EXPECT_THROW( imageWriter3D_A->Update(), itk::ExceptionObject );
}
}
