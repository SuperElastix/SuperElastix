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
    selx::AnyFileReader::Pointer anyReader;
    EXPECT_NO_THROW(anyReader = image2DReader);

    anyReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

    itk::DataObject::Pointer anyOutput = anyReader->GetOutput();

    anyReader->Update();

    Image2DType::Pointer image2DOutput = dynamic_cast<Image2DType*>(anyOutput.GetPointer());

    EXPECT_FALSE(image2DOutput == nullptr);

    //ImageReaderType::Pointer imageReader = ImageReaderType::New();
    //ImageWriterType::Pointer imageWriter = ImageWriterType::New();

    //imageReader->SetFileName("in2dimage.mhd");
    //imageWriter->SetFileName("out2dimage.mhd");

    Mesh2DReaderType::Pointer mesh2DReader = Mesh2DReaderType::New();
    Mesh2DWriterType::Pointer mesh2DWriter = Mesh2DWriterType::New();

    mesh2DReader->SetFileName(dataManager->GetInputFile("in2dmesh.vtk"));
    mesh2DWriter->SetFileName(dataManager->GetInputFile("out2dmesh.vtk"));
  }
}