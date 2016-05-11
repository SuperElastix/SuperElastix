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

#include "gtest/gtest.h"

namespace selx {

  class SuperElastixFilterTest : public ::testing::Test {
  public:
    typedef itk::Image<float, 2> ImageType;
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::ImageFileWriter<ImageType> ImageWriterType;

    typedef float dummyType;
    typedef dummyType FixedType;
    typedef dummyType MovingType;
    typedef selx::SuperElastixFilter < FixedType, MovingType > SuperElastixFilterType;

    typedef itk::Mesh<float, 2> MeshType;
    typedef itk::MeshFileReader<MeshType> MeshReaderType;
    typedef itk::MeshFileWriter<MeshType> MeshWriterType;

    virtual void SetUp() {

    }

    virtual void TearDown() {

    }

  };

  TEST_F(SuperElastixFilterTest, IO)
  {
    ImageReaderType::Pointer imageReader = ImageReaderType::New();
    ImageWriterType::Pointer imageWriter = ImageWriterType::New();
    imageReader->SetFileName("in2dimage.mhd");
    imageWriter->SetFileName("out2dimage.mhd");

    MeshReaderType::Pointer meshReader = MeshReaderType::New();
    MeshWriterType::Pointer meshWriter = MeshWriterType::New();

    meshReader->SetFileName("in2dmesh.vtk");
    meshWriter->SetFileName("out2dmesh.vtk");

    auto mesh = meshReader->GetOutput();

    SuperElastixFilterType::Pointer mySuperElastix;
    EXPECT_NO_THROW(mySuperElastix = SuperElastixFilterType::New());
    mySuperElastix->SetConfiguration();
    mySuperElastix->SetInput("FixedImage", imageReader->GetOutput());
    imageWriter->SetInput(mySuperElastix->GetOutput<ImageType>("ResultImage"));
    //imageWriter->SetInput(imageReader->GetOutput());
    mySuperElastix->SetInput("FixedMesh", meshReader->GetOutput());
    meshWriter->SetInput(mySuperElastix->GetOutput<MeshType>("ResultMesh"));
    //meshWriter->SetInput(meshReader->GetOutput());
    EXPECT_NO_THROW(imageWriter->Update());
    EXPECT_NO_THROW(meshWriter->Update());

  }
}