#include "elxElastixFilter.h"
#include "elxTransformixFilter.h"
#include "elxParameterObject.h"

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

using namespace elastix;

class TransformixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  typedef ParameterObject::ParameterValueVectorType ParameterValueVectorType;
  typedef ParameterObject::ParameterMapType ParameterMapType;

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  typedef TransformixFilter< ImageType > TransformixFilterType;
};

TEST_F( TransformixFilterTest, Instantiation )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  EXPECT_NO_THROW( ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New() );
}

TEST_F( TransformixFilterTest, Euler2D )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  elastixFilter->LogToConsoleOn();

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( transformixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );
}

TEST_F( TransformixFilterTest, GetElastixOutputEuler2D )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  transformixFilter->Update();
  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( transformixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );
}


TEST_F( TransformixFilterTest, UpdateOnGetOutput )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( transformixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );
}

TEST_F( TransformixFilterTest, UpdateOnGetTransformParameterObject )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( ParameterObject::Pointer transformParameters = transformixFilter->GetTransformParameterObject() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( transformixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );
}

TEST_F( TransformixFilterTest, ComputeSpatialJacobian )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );


  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->ComputeSpatialJacobianOn() );
  transformixFilter->Update();

}

TEST_F( TransformixFilterTest, ComputeDeterminantOfSpatialJacobian )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->ComputeDeterminantOfSpatialJacobianOn() );
  EXPECT_NO_THROW( transformixFilter->Update() );
}

TEST_F( TransformixFilterTest, ComputeDeformationField )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->ComputeDeformationFieldOn() );
  EXPECT_NO_THROW( transformixFilter->Update() );
}

TEST_F( TransformixFilterTest, TransformPointSet )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  // We generate point set manually
  std::ofstream fixedMeshFile;
  fixedMeshFile.open( dataManager->GetInputFile( "InputPoints.pts" ));
  fixedMeshFile << "point\n";
  fixedMeshFile << "1\n";
  fixedMeshFile << "128.0 128.0\n";
  fixedMeshFile.close();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->SetInputPointSetFileName( dataManager->GetInputFile( "InputPoints.pts" ) ) );
  EXPECT_NO_THROW( transformixFilter->Update() );
}

TEST_F( TransformixFilterTest, SameTransformParameterMapForMultipleTransformations )
{ 
  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader1 = ImageFileReaderType::New();
  movingImageReader1->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ImageFileReaderType::Pointer movingImageReader2 = ImageFileReaderType::New();
  movingImageReader2->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader1->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->Update() );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameterObject() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader1->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->Update() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader2->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->Update() );
}

#ifdef SUPERELASTIX_BUILD_LONG_TESTS

TEST_F( TransformixFilterTest, BSpline4D )
{
  ParameterObject::Pointer parameterObject = ParameterObject::New();
  parameterObject->SetParameterMap( "groupwise" );
  parameterObject->GetParameterMap( 0 )[ "MaximumNumberOfIterations" ] = ParameterValueVectorType( 1, "4" );

  typedef itk::Image< float, 4 > FloatImageType;
  typedef itk::ImageFileReader< FloatImageType > ImageFileReaderType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer imageReader = ImageFileReaderType::New();
  imageReader->SetFileName( dataManager->GetInputFile( "4D.nii.gz" ) );
  imageReader->Update();

  // Elastix is not compiled with the combination of float and dim = 4 by default
  typedef itk::Image< short, 4 > ShortImageType;
  typedef itk::CastImageFilter< FloatImageType, ShortImageType > CastImageFilterType;
  typedef itk::ImageFileWriter< ShortImageType > ImageFileWriterType;
  typedef ElastixFilter< ShortImageType, ShortImageType > ElastixFilterType;

  CastImageFilterType::Pointer castImageFilter = CastImageFilterType::New();
  castImageFilter->SetInput( imageReader->GetOutput() );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( castImageFilter->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( castImageFilter->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "BSpline4DResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );

  ParameterObject::Pointer transformParameterObject;
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameterObject() );
}

#endif // SUPERELASTIX_BUILD_LONG_TESTS
