#include "elxElastixFilter.h"
#include "elxTransformixFilter.h"
#include "elxParameterObject.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

using namespace selx;

class TransformixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  typedef ParameterObject::ParameterVectorType              ParameterVectorType;
  typedef ParameterObject::ParameterMapType                 ParameterMapType;

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  typedef TransformixFilter< ImageType > TransformixFilterType;

  ElastixFilterType::Pointer elastixFilter;
  ParameterObject::Pointer eulerTransformParameterObject;

  virtual void SetUp()
  {
    // Nonrigid ParameterMap
    ParameterMapType parameterMap                       = ParameterMapType();

    // Images
    parameterMap[ "FixedInternalImagePixelType" ]       = ParameterVectorType( 1, "float" );
    parameterMap[ "FixedImageDimension" ]               = ParameterVectorType( 1, "2" );
    parameterMap[ "MovingInternalImagePixelType" ]      = ParameterVectorType( 1, "float" );
    parameterMap[ "MovingImageDimension" ]              = ParameterVectorType( 1, "2" );
    parameterMap[ "ResultImagePixelType" ]              = ParameterVectorType( 1, "float" );

    // Common components
    parameterMap[ "FixedImagePyramid" ]                 = ParameterVectorType( 1, "FixedSmoothingImagePyramid" );
    parameterMap[ "MovingImagePyramid" ]                = ParameterVectorType( 1, "MovingSmoothingImagePyramid" );
    parameterMap[ "Interpolator"]                       = ParameterVectorType( 1, "LinearInterpolator");
    parameterMap[ "Optimizer" ]                         = ParameterVectorType( 1, "AdaptiveStochasticGradientDescent" );
    parameterMap[ "Resampler"]                          = ParameterVectorType( 1, "DefaultResampler" );
    parameterMap[ "ResampleInterpolator"]               = ParameterVectorType( 1, "FinalLinearInterpolator" );
    parameterMap[ "FinalBSplineInterpolationOrder" ]    = ParameterVectorType( 1, "2" );
    parameterMap[ "NumberOfResolutions" ]               = ParameterVectorType( 1, "2" );

    // Image Sampler
    parameterMap[ "ImageSampler" ]                      = ParameterVectorType( 1, "RandomCoordinate" ); 
    parameterMap[ "NumberOfSpatialSamples"]             = ParameterVectorType( 1, "2048" );
    parameterMap[ "CheckNumberOfSamples" ]              = ParameterVectorType( 1, "true" );
    parameterMap[ "MaximumNumberOfSamplingAttempts" ]   = ParameterVectorType( 1, "8" );
    parameterMap[ "NewSamplesEveryIteration" ]          = ParameterVectorType( 1, "true");

    // Optimizer
    parameterMap[ "NumberOfSamplesForExactGradient" ]   = ParameterVectorType( 1, "4096" );
    parameterMap[ "DefaultPixelValue" ]                 = ParameterVectorType( 1, "0" );
    parameterMap[ "AutomaticParameterEstimation" ]      = ParameterVectorType( 1, "true" );

    // Output
    parameterMap[ "WriteResultImage" ]                  = ParameterVectorType( 1, "true" );
    parameterMap[ "ResultImageFormat" ]                 = ParameterVectorType( 1, "nii" );

    // Registration
    parameterMap[ "Registration" ]                      = ParameterVectorType( 1, "MultiResolutionRegistration" );
    parameterMap[ "Transform" ]                         = ParameterVectorType( 1, "EulerTransform" );
    parameterMap[ "Metric" ]                            = ParameterVectorType( 1, "AdvancedMattesMutualInformation" );
    parameterMap[ "MaximumNumberOfIterations" ]         = ParameterVectorType( 1, "128" );

    eulerTransformParameterObject = ParameterObject::New();
    eulerTransformParameterObject->SetParameterMap( parameterMap );

    // We generate the point sets manually
    DataManagerType::Pointer dataManager = DataManagerType::New();
    std::ofstream inputMeshFile;
    inputMeshFile.open( dataManager->GetInputFile( "InputMesh.pts" ));
    inputMeshFile << "point\n";
    inputMeshFile << "1\n";
    inputMeshFile << "115.0 111.0\n";
    inputMeshFile.close();
  }
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
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( eulerTransformParameterObject ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameters() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( transformixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) );
  writer->SetInput( transformixFilter->GetOutput() );
  writer->Update();
}


TEST_F( TransformixFilterTest, UpdateOnGetOutput )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( eulerTransformParameterObject ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameters() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) );
  writer->SetInput( transformixFilter->GetOutput() );
  writer->Update();
}

TEST_F( TransformixFilterTest, UpdateOnGetTransformParameters )
{
  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( eulerTransformParameterObject ) );

  TransformixFilterType::Pointer transformixFilter;
  EXPECT_NO_THROW( transformixFilter = TransformixFilterType::New() );
  EXPECT_NO_THROW( transformixFilter->SetInputImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( transformixFilter->SetOutputDirectory( dataManager->GetOutputDirectory() ) );
  EXPECT_NO_THROW( transformixFilter->SetTransformParameterObject( elastixFilter->GetTransformParameters() ) );
  EXPECT_NO_THROW( transformixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( transformixFilter->LogToFileOn() );
  EXPECT_NO_THROW( ParameterObject::Pointer transformParameters = transformixFilter->GetTransformParameters() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "Euler2DTransformixResultImage.nii" ) );
  writer->SetInput( transformixFilter->GetOutput() );
  writer->Update();
}

// TODO: Write tests for
// EXPECT_NO_THROW( transformixFilter->ComputeSpatialJacobianOn() );
// EXPECT_NO_THROW( transformixFilter->ComputeDeterminantOfSpatialJacobianOn() );
// EXPECT_NO_THROW( transformixFilter->ComputeDeformationFieldOn() );
// EXPECT_NO_THROW( transformixFilter->SetPointSetFileName( dataManager->GetInputFile( "InputMesh.pts" ) ) );
