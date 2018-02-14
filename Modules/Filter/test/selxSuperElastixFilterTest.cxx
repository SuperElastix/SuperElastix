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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "selxItkMeshSinkComponent.h"
#include "selxItkMeshSourceComponent.h"

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

  /** Fill SuperElastix' component data base by registering various components */
  typedef TypeList<
    ItkImageSinkComponent< 3, double >,
    ItkImageSourceComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, double >,
    ItkMeshSinkComponent< 2, float >,
    ItkMeshSourceComponent< 2, float >> CustomComponents;

  using RegisterComponents = list_append< CustomComponents, DefaultComponents >;

  typedef Blueprint::Pointer BlueprintPointer;
  typedef Logger::Pointer LoggerPointer;

  typedef itk::Mesh< float, 2 >           MeshType;
  typedef itk::MeshFileReader< MeshType > MeshReaderType;
  typedef itk::MeshFileWriter< MeshType > MeshWriterType;

  typedef DataManager DataManagerType;
  virtual void SetUp()
  {
    dataManager = DataManagerType::New();
    logger = Logger::New();
    logger->SetLogLevel( LogLevel::DBG );
    logger->AddStream( "cout", std::cout );
  }


  virtual void TearDown()
  {
    // Unregister all components after each test
    // TODO: when SuperElastix is refactored to not use the ITK object factory this UnRegisterAllFactories call must be removed
    // itk::ObjectFactoryBase::UnRegisterAllFactories();
    // Delete the SuperElastixFilter after each test
  }


  // BlueprintImpl holds a configuration for SuperElastix
  BlueprintPointer blueprint;
  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager;
  // Logger
  LoggerPointer logger;

};

TEST_F( SuperElastixFilterTest, ImageOnly )
{
  ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
  ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

  imageReader3D->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageWriter3D->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_sphereA3d.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetComponent( "ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } } );
  blueprint->SetComponent( "OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetConnection( "InputImage", "ImageFilter", { {} } ); //
  blueprint->SetConnection( "ImageFilter", "OutputImage", { {} } );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger( logger );
  superElastixFilter->SetBlueprint( blueprint );
  superElastixFilter->SetInput( "InputImage", imageReader3D->GetOutput() );
  imageWriter3D->SetInput( superElastixFilter->GetOutput< Image3DType >( "OutputImage" ) );

  imageWriter3D->Update();
}
TEST_F( SuperElastixFilterTest, ImageAndMesh )
{
  ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
  ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

  imageReader3D->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageWriter3D->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_sphereA3d.mhd" ) );

  MeshReaderType::Pointer meshReader = MeshReaderType::New();
  MeshWriterType::Pointer meshWriter = MeshWriterType::New();

  meshReader->SetFileName( dataManager->GetInputFile( "2dSquare.vtk" ) );
  meshWriter->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_2dSquare.vtk" ) );

  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetComponent( "ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } } );
  blueprint->SetComponent( "OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetConnection( "InputImage", "ImageFilter", BlueprintImpl::ParameterMapType() ); //
  blueprint->SetConnection( "ImageFilter", "OutputImage", BlueprintImpl::ParameterMapType() );

  blueprint->SetComponent( "InputMesh", { { "NameOfClass", { "ItkMeshSourceComponent" } } } );
  blueprint->SetComponent( "OutputMesh", { { "NameOfClass", { "ItkMeshSinkComponent" } } } );
  blueprint->SetConnection( "InputMesh", "OutputMesh", BlueprintImpl::ParameterMapType() );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  superElastixFilter->SetBlueprint( blueprint );
  superElastixFilter->SetInput( "InputMesh", meshReader->GetOutput() );
  meshWriter->SetInput( superElastixFilter->GetOutput< MeshType >( "OutputMesh" ) );

  superElastixFilter->SetInput( "InputImage", imageReader3D->GetOutput() );
  imageWriter3D->SetInput( superElastixFilter->GetOutput< Image3DType >( "OutputImage" ) );

  EXPECT_NO_THROW( meshWriter->Update() );
  EXPECT_NO_THROW( imageWriter3D->Update() );
}
TEST_F( SuperElastixFilterTest, TooManyInputs )
{
  ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();
  ImageReader3DType::Pointer imageReader3D_B = ImageReader3DType::New();

  imageReader3D_A->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );
  imageReader3D_B->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  superElastixFilter->SetInput( "Source_A", imageReader3D_A->GetOutput() );
  superElastixFilter->SetInput( "Source_B", imageReader3D_B->GetOutput() );

  EXPECT_THROW( superElastixFilter->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManySources )
{
  ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();

  imageReader3D_A->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetComponent( "Source_B", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );
  superElastixFilter->SetInput( "Source_A", imageReader3D_A->GetOutput() );

  EXPECT_THROW( superElastixFilter->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManyOutputs )
{
  ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
  ImageWriter3DType::Pointer imageWriter3D_B = ImageWriter3DType::New();
  imageWriter3D_A->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );
  imageWriter3D_B->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  superElastixFilter->SetBlueprint( blueprint );
  imageWriter3D_A->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_A" ) );
  imageWriter3D_B->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_B" ) );

  EXPECT_THROW( imageWriter3D_B->Update(), itk::ExceptionObject );
}
TEST_F( SuperElastixFilterTest, TooManySinks )
{
  ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
  imageWriter3D_A->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_TooManyOutputs.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetComponent( "Sink_B", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );

  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  superElastixFilter->SetBlueprint( blueprint );

  imageWriter3D_A->SetInput( superElastixFilter->GetOutput< Image3DType >( "Sink_A" ) );

  EXPECT_THROW( imageWriter3D_A->Update(), itk::ExceptionObject );
}

TEST_F( SuperElastixFilterTest, UnsatisfiedConnections )
{
  //ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
  ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

  //imageReader3D->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));
  imageWriter3D->SetFileName( dataManager->GetOutputFile( "SuperElastixFilterTest_sphereA3d.mhd" ) );

  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetComponent( "OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } } );
  blueprint->SetConnection( "ImageFilter", "OutputImage", { {} } );

  // Instantiate SuperElastixFilter before each test and
  // register the components we want to have available in SuperElastix
  SuperElastixFilterCustomComponents< RegisterComponents >::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New() );
  superElastixFilter->SetLogger(logger);
  superElastixFilter->SetBlueprint( blueprint );

  //superElastixFilter->SetInput("InputImage", imageReader3D->GetOutput());
  imageWriter3D->SetInput( superElastixFilter->GetOutput< Image3DType >( "OutputImage" ) );

  EXPECT_THROW( imageWriter3D->Update(), itk::ExceptionObject );
}
}
