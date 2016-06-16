#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx {

  class AnyFileIOTest : public ::testing::Test
  {
  public:

    typedef itk::Image<float, 2> Image2DType;
    typedef itk::ImageFileReader<Image2DType> Image2DReaderType;
    typedef itk::ImageFileWriter<Image2DType> Image2DWriterType;
    typedef FileReaderDecorator<Image2DReaderType> DecoratedImage2DReaderType;

    typedef itk::Image<double, 3> Image3DType;
    typedef itk::ImageFileReader<Image3DType> Image3DReaderType;
    typedef itk::ImageFileWriter<Image3DType> Image3DWriterType;
    typedef FileReaderDecorator<Image3DReaderType> DecoratedImage3DReaderType;

    typedef itk::Mesh<float, 2> Mesh2DType;
    typedef itk::MeshFileReader<Mesh2DType> Mesh2DReaderType;
    typedef itk::MeshFileWriter<Mesh2DType> Mesh2DWriterType;
    typedef FileReaderDecorator<Mesh2DReaderType> DecoratedMesh2DReaderType;

    typedef DataManager DataManagerType;
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

  };

  TEST_F(AnyFileIOTest, Reader)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    DecoratedImage2DReaderType::Pointer image2DReader;
    EXPECT_NO_THROW(image2DReader = DecoratedImage2DReaderType::New());

    // assign the reader for 2d images to an agnostic reader type 
    AnyFileReader::Pointer anyReader1;
    EXPECT_NO_THROW(anyReader1 = image2DReader);

    anyReader1->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

    // anyReaders output is a generic DataObject (i.e. the base class of itk image, meshes, etc.)
    itk::DataObject::Pointer anyOutput1 = anyReader1->GetOutput();
    EXPECT_NO_THROW(anyReader1->Update());

    // see if the anyReader delegated the actual data reading to a reader of the right type.
    Image2DType::Pointer image2DOutput = dynamic_cast<Image2DType*>(anyOutput1.GetPointer());
    EXPECT_FALSE(image2DOutput == nullptr);


    DecoratedMesh2DReaderType::Pointer mesh2DReader;
    EXPECT_NO_THROW(mesh2DReader = DecoratedMesh2DReaderType::New());

    // assign the reader for 2d meshes to an agnostic reader type 
    AnyFileReader::Pointer anyReader2;
    EXPECT_NO_THROW(anyReader2 = mesh2DReader);

    anyReader2->SetFileName(dataManager->GetInputFile("in2dmesh.vtk"));

    // anyReaders output is a generic DataObject (i.e. the base class of itk image, meshes, etc.)
    itk::DataObject::Pointer anyOutput2 = anyReader2->GetOutput();
    EXPECT_NO_THROW(anyReader2->Update());

    // see if the anyReader delegated the actual data reading to a reader of the right type.
    Mesh2DType::Pointer mesh2DOutput = dynamic_cast<Mesh2DType*>(anyOutput2.GetPointer());
    EXPECT_FALSE(mesh2DOutput == nullptr);



    DecoratedImage3DReaderType::Pointer image3DReader;
    EXPECT_NO_THROW(image3DReader = DecoratedImage3DReaderType::New());

    // assign the reader for 3d images to an agnostic reader type 
    AnyFileReader::Pointer anyReader3;
    EXPECT_NO_THROW(anyReader3 = image3DReader);

    anyReader3->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));

    // anyReaders output is a generic DataObject (i.e. the base class of itk image, meshes, etc.)
    itk::DataObject::Pointer anyOutput3 = anyReader3->GetOutput();
    EXPECT_NO_THROW(anyReader3->Update());

    // see if the anyReader delegated the actual data reading to a reader of the right type.
    Image3DType::Pointer image3DOutput = dynamic_cast<Image3DType*>(anyOutput3.GetPointer());
    EXPECT_FALSE(image3DOutput == nullptr);

  }
}