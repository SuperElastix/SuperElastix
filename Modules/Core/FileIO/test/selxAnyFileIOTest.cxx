#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

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
    typedef FileWriterDecorator<Image2DWriterType> DecoratedImage2DWriterType;

    typedef itk::Image<double, 3> Image3DType;
    typedef itk::ImageFileReader<Image3DType> Image3DReaderType;
    typedef itk::ImageFileWriter<Image3DType> Image3DWriterType;
    typedef FileReaderDecorator<Image3DReaderType> DecoratedImage3DReaderType;
    typedef FileWriterDecorator<Image3DWriterType> DecoratedImage3DWriterType;

    typedef itk::Mesh<float, 2> Mesh2DType;
    typedef itk::MeshFileReader<Mesh2DType> Mesh2DReaderType;
    typedef itk::MeshFileWriter<Mesh2DType> Mesh2DWriterType;
    typedef FileReaderDecorator<Mesh2DReaderType> DecoratedMesh2DReaderType;
    typedef FileWriterDecorator<Mesh2DWriterType> DecoratedMesh2DWriterType;

    typedef DataManager DataManagerType;
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

  };

  TEST_F(AnyFileIOTest, Readers)
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
  TEST_F(AnyFileIOTest, Writers)
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    // Create a dataObject of a known type by reading from disk;
    Image2DReaderType::Pointer image2DReader = Image2DReaderType::New();
    image2DReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

    DecoratedImage2DWriterType::Pointer image2DWriter;
    EXPECT_NO_THROW(image2DWriter = DecoratedImage2DWriterType::New());

    // assign the writer for 2d images to an agnostic writer type 
    AnyFileWriter::Pointer anyWriter1;
    EXPECT_NO_THROW(anyWriter1 = image2DWriter);

    anyWriter1->SetFileName(dataManager->GetOutputFile("AnyFileIOTest_Writers_coneA2d64.mhd"));

    // anyWriter input is a generic DataObject, but with help of polymorphism it gets casted to the DataType of the decorated writer
    anyWriter1->SetInput(image2DReader->GetOutput()); 
    EXPECT_NO_THROW(anyWriter1->Update());


    //TODO: mesh writer requires the FileWriterDecorator to be templated with traits class, see FileWriterDecorator.h


    // Create a dataObject of a known type by reading from disk;
    Image3DReaderType::Pointer image3DReader = Image3DReaderType::New();
    image3DReader->SetFileName(dataManager->GetInputFile("sphereA3d.mhd"));

    DecoratedImage3DWriterType::Pointer image3DWriter;
    EXPECT_NO_THROW(image3DWriter = DecoratedImage3DWriterType::New());

    // assign the writer for 3d images to an agnostic writer type 
    AnyFileWriter::Pointer anyWriter3;
    EXPECT_NO_THROW(anyWriter3 = image3DWriter);

    anyWriter3->SetFileName(dataManager->GetOutputFile("AnyFileIOTest_Writers_sphereA3d.mhd"));

    // anyWriter input is a generic DataObject, but with help of polymorphism it gets casted to the DataType of the decorated writer
    anyWriter3->SetInput(image3DReader->GetOutput()); 
    EXPECT_NO_THROW(anyWriter3->Update());

  }
}