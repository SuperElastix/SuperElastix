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

  typedef ParameterObject::ParameterValueVectorType ParameterValueVectorType;
  typedef ParameterObject::ParameterMapType ParameterMapType;

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
    parameterMap[ "FixedInternalImagePixelType" ]       = ParameterValueVectorType( 1, "float" );
    parameterMap[ "FixedImageDimension" ]               = ParameterValueVectorType( 1, "2" );
    parameterMap[ "MovingInternalImagePixelType" ]      = ParameterValueVectorType( 1, "float" );
    parameterMap[ "MovingImageDimension" ]              = ParameterValueVectorType( 1, "2" );
    parameterMap[ "ResultImagePixelType" ]              = ParameterValueVectorType( 1, "float" );

    // Common components
    parameterMap[ "FixedImagePyramid" ]                 = ParameterValueVectorType( 1, "FixedSmoothingImagePyramid" );
    parameterMap[ "MovingImagePyramid" ]                = ParameterValueVectorType( 1, "MovingSmoothingImagePyramid" );
    parameterMap[ "Interpolator"]                       = ParameterValueVectorType( 1, "LinearInterpolator");
    parameterMap[ "Optimizer" ]                         = ParameterValueVectorType( 1, "AdaptiveStochasticGradientDescent" );
    parameterMap[ "Resampler"]                          = ParameterValueVectorType( 1, "DefaultResampler" );
    parameterMap[ "ResampleInterpolator"]               = ParameterValueVectorType( 1, "FinalLinearInterpolator" );
    parameterMap[ "FinalBSplineInterpolationOrder" ]    = ParameterValueVectorType( 1, "2" );
    parameterMap[ "NumberOfResolutions" ]               = ParameterValueVectorType( 1, "2" );

    // Image Sampler
    parameterMap[ "ImageSampler" ]                      = ParameterValueVectorType( 1, "RandomCoordinate" ); 
    parameterMap[ "NumberOfSpatialSamples"]             = ParameterValueVectorType( 1, "2048" );
    parameterMap[ "CheckNumberOfSamples" ]              = ParameterValueVectorType( 1, "true" );
    parameterMap[ "MaximumNumberOfSamplingAttempts" ]   = ParameterValueVectorType( 1, "8" );
    parameterMap[ "NewSamplesEveryIteration" ]          = ParameterValueVectorType( 1, "true");

    // Optimizer
    parameterMap[ "NumberOfSamplesForExactGradient" ]   = ParameterValueVectorType( 1, "4096" );
    parameterMap[ "DefaultPixelValue" ]                 = ParameterValueVectorType( 1, "0" );
    parameterMap[ "AutomaticParameterEstimation" ]      = ParameterValueVectorType( 1, "true" );

    // Output
    parameterMap[ "WriteResultImage" ]                  = ParameterValueVectorType( 1, "true" );
    parameterMap[ "ResultImageFormat" ]                 = ParameterValueVectorType( 1, "nii" );

    // Registration
    parameterMap[ "Registration" ]                      = ParameterValueVectorType( 1, "MultiResolutionRegistration" );
    parameterMap[ "Transform" ]                         = ParameterValueVectorType( 1, "EulerTransform" );
    parameterMap[ "Metric" ]                            = ParameterValueVectorType( 1, "AdvancedMattesMutualInformation" );
    parameterMap[ "MaximumNumberOfIterations" ]         = ParameterValueVectorType( 1, "128" );

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

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

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

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

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

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

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
