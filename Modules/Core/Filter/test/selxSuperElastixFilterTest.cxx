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

namespace selx {

  class SuperElastixFilterTest : public ::testing::Test {
  public:
    typedef itk::Image<float, 2> Image2DType;
    typedef itk::ImageFileReader<Image2DType> ImageReader2DType;
    typedef itk::ImageFileWriter<Image2DType> ImageWriter2DType;

    typedef itk::Image<double, 3> Image3DType;
    typedef itk::ImageFileReader<Image3DType> ImageReader3DType;
    typedef itk::ImageFileWriter<Image3DType> ImageWriter3DType;

    /** Fill SUPERelastix' component data base by registering various components */
    typedef TypeList <
      ItkImageSinkComponent<3, double>,
      ItkImageSourceComponent<3, double>,
      ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>,
      ItkMeshSinkComponent<2, float>,
      ItkMeshSourceComponent<2, float> > RegisterComponents;

    typedef SuperElastixFilter<RegisterComponents>          SuperElastixFilterType;
    
    typedef itk::Mesh<float, 2> MeshType;
    typedef itk::MeshFileReader<MeshType> MeshReaderType;
    typedef itk::MeshFileWriter<MeshType> MeshWriterType;

    typedef DataManager DataManagerType;
    virtual void SetUp() {

    }

    virtual void TearDown() {
      itk::ObjectFactoryBase::UnRegisterAllFactories();
    }

  };

  TEST_F(SuperElastixFilterTest, ImageOnly)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
    ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();
    
    imageReader3D->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));
    imageWriter3D->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_sphereA3d.mhd"));
   
    Blueprint::Pointer blueprint = Blueprint::New();
   
    blueprint->AddComponent("InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    blueprint->AddComponent("ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } });
    blueprint->AddComponent("OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } });
    blueprint->AddConnection("InputImage", "ImageFilter", { {} }); // 
    blueprint->AddConnection("ImageFilter", "OutputImage", { {} });

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    mySuperElastix->SetInput("InputImage", imageReader3D->GetOutput());
    imageWriter3D->SetInput(mySuperElastix->GetOutput<Image3DType>("OutputImage"));
    
    EXPECT_NO_THROW(imageWriter3D->Update());
  }
  TEST_F(SuperElastixFilterTest, ImageAndMesh)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReader3DType::Pointer imageReader3D = ImageReader3DType::New();
    ImageWriter3DType::Pointer imageWriter3D = ImageWriter3DType::New();

    imageReader3D->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));
    imageWriter3D->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_sphereA3d.mhd"));

    MeshReaderType::Pointer meshReader = MeshReaderType::New();
    MeshWriterType::Pointer meshWriter = MeshWriterType::New();

    meshReader->SetFileName(dataManager->GetInputFile("2dSquare.vtk"));
    meshWriter->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_2dSquare.vtk"));

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    blueprint->AddComponent("ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } });
    blueprint->AddComponent("OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } });
    blueprint->AddConnection("InputImage", "ImageFilter", Blueprint::ParameterMapType()); // 
    blueprint->AddConnection("ImageFilter", "OutputImage", Blueprint::ParameterMapType());


    blueprint->AddComponent("InputMesh", { { "NameOfClass", { "ItkMeshSourceComponent" } } });
    blueprint->AddComponent("OutputMesh", { { "NameOfClass", { "ItkMeshSinkComponent" } } });
    blueprint->AddConnection("InputMesh", "OutputMesh", Blueprint::ParameterMapType());

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    mySuperElastix->SetInput("InputMesh", meshReader->GetOutput());  
    meshWriter->SetInput(mySuperElastix->GetOutput<MeshType>("OutputMesh"));

    mySuperElastix->SetInput("InputImage", imageReader3D->GetOutput());
    imageWriter3D->SetInput(mySuperElastix->GetOutput<Image3DType>("OutputImage"));

    //mySuperElastix->Update();

    EXPECT_NO_THROW(meshWriter->Update());
    EXPECT_NO_THROW(imageWriter3D->Update());
  }
  TEST_F(SuperElastixFilterTest, TooManyInputs)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();
    ImageReader3DType::Pointer imageReader3D_B = ImageReader3DType::New();

    imageReader3D_A->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));
    imageReader3D_B->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } } });

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    mySuperElastix->SetInput("Source_A", imageReader3D_A->GetOutput());
    mySuperElastix->SetInput("Source_B", imageReader3D_B->GetOutput());

    EXPECT_THROW(mySuperElastix->Update(), itk::ExceptionObject);

  }
  TEST_F(SuperElastixFilterTest, TooManySources)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReader3DType::Pointer imageReader3D_A = ImageReader3DType::New();

    imageReader3D_A->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("Source_A", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    blueprint->AddComponent("Source_B", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    mySuperElastix->SetInput("Source_A", imageReader3D_A->GetOutput());

    EXPECT_THROW(mySuperElastix->Update(), itk::ExceptionObject);

  }
  TEST_F(SuperElastixFilterTest, TooManyOutputs)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
    ImageWriter3DType::Pointer imageWriter3D_B = ImageWriter3DType::New();
    imageWriter3D_A->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_TooManyOutputs.mhd"));
    imageWriter3D_B->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_TooManyOutputs.mhd"));

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } } });

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    imageWriter3D_A->SetInput(mySuperElastix->GetOutput<Image3DType>("Sink_A"));
    imageWriter3D_B->SetInput(mySuperElastix->GetOutput<Image3DType>("Sink_B"));
    

    EXPECT_THROW(imageWriter3D_B->Update(), itk::ExceptionObject);

  }
  TEST_F(SuperElastixFilterTest, TooManySinks)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageWriter3DType::Pointer imageWriter3D_A = ImageWriter3DType::New();
    imageWriter3D_A->SetFileName(dataManager->GetOutputFile("SuperElastixFilterTest_TooManyOutputs.mhd"));

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("Sink_A", { { "NameOfClass", { "ItkImageSinkComponent" } } });
    blueprint->AddComponent("Sink_B", { { "NameOfClass", { "ItkImageSinkComponent" } } });

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetBlueprint(blueprint);

    imageWriter3D_A->SetInput(mySuperElastix->GetOutput<Image3DType>("Sink_A"));

    EXPECT_THROW(imageWriter3D_A->Update(), itk::ExceptionObject);

  }
}