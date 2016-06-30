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

    //TODO make SuperElastixFilter templated over Components
    typedef selx::SuperElastixFilter < bool > SuperElastixFilterType;

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

    ComponentFactory<ItkImageSinkComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>>::RegisterOneFactory();

    SuperElastixFilterType::Pointer mySuperElastix;
    
    Blueprint::Pointer blueprint = Blueprint::New();
   
    blueprint->AddComponent("InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    blueprint->AddComponent("ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } });
    blueprint->AddComponent("OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } });
    blueprint->AddConnection("InputImage", "ImageFilter", Blueprint::ParameterMapType()); // 
    blueprint->AddConnection("ImageFilter", "OutputImage", Blueprint::ParameterMapType());

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


    ComponentFactory<ItkImageSinkComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceComponent<3, double>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>>::RegisterOneFactory();

    ComponentFactory<ItkMeshSinkComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkMeshSourceComponent<2, float>>::RegisterOneFactory();

    SuperElastixFilterType::Pointer mySuperElastix;

    Blueprint::Pointer blueprint = Blueprint::New();

    blueprint->AddComponent("InputImage", { { "NameOfClass", { "ItkImageSourceComponent" } } });
    blueprint->AddComponent("ImageFilter", { { "NameOfClass", { "ItkSmoothingRecursiveGaussianImageFilterComponent" } } });
    blueprint->AddComponent("OutputImage", { { "NameOfClass", { "ItkImageSinkComponent" } } });
    blueprint->AddConnection("InputImage", "ImageFilter", Blueprint::ParameterMapType()); // 
    blueprint->AddConnection("ImageFilter", "OutputImage", Blueprint::ParameterMapType());


    blueprint->AddComponent("InputMesh", { { "NameOfClass", { "ItkMeshSourceComponent" } } });
    blueprint->AddComponent("OutputMesh", { { "NameOfClass", { "ItkMeshSinkComponent" } } });
    blueprint->AddConnection("InputMesh", "OutputMesh", Blueprint::ParameterMapType());

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
}