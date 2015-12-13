#include "elxElastixFilter.h"
#include "elxParameterObject.h"

#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

using namespace selx;

class ElastixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  // Mesh typedef 
  typedef itk::Mesh< float, 2u >                            MeshType;
  typedef itk::MeshFileReader< MeshType >                   MeshReaderType;

  // Image typedefs
  typedef itk::Image< float, 2u >                           ImageType;
  typedef itk::ImageFileReader< ImageType >                 ImageReaderType;
  typedef itk::ImageFileWriter< ImageType >                 ImageWriterType;

  // Parameter typedefs
  typedef ParameterObject::ParameterVectorType              ParameterVectorType;
  typedef ParameterObject::ParameterMapType                 ParameterMapType;

  // Elastix typedefs
  typedef ElastixFilter< ImageType, ImageType >             ElastixFilterType;
  typedef ElastixFilterType::DataObjectContainerType        DataObjectContainerType;
  typedef ElastixFilterType::DataObjectContainerPointer     DataObjectContainerPointer;

  ImageType::Pointer fixedImage;
  ImageType::Pointer movingImage;
  ImageType::Pointer resultImage;

  std::string fixedMeshFileName;
  std::string movingMeshFileName;

  ParameterMapType parameterMap;
  ParameterObject::Pointer parameterObject;
  ParameterObject::Pointer correspondingPointsParameterObject;
  ParameterObject::ConstPointer transformParameterObject;

  virtual void SetUp()
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    // TODO: All calls to update here should not be needed but propagated to reader by ElastixFilter

    // Input images
    ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
    fixedImageReader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    fixedImage = fixedImageReader->GetOutput();
    fixedImageReader->Update();

    ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
    movingImageReader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    movingImage = movingImageReader->GetOutput();
    movingImageReader->Update();

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

    parameterObject = ParameterObject::New();
    parameterObject->SetParameterMap( parameterMap );

    ParameterMapType correspondingPointsParameterMap = parameterMap;
    correspondingPointsParameterMap[ "Registration" ] = ParameterVectorType( 1, "MultiMetricMultiResolutionRegistration" ); 
    correspondingPointsParameterMap[ "Metric" ].push_back( "CorrespondingPointsEuclideanDistanceMetric" );
    correspondingPointsParameterObject = ParameterObject::New();
    correspondingPointsParameterObject->SetParameterMap( correspondingPointsParameterMap );

    fixedMeshFileName = dataManager->GetInputFile( "bioid_0000.pts" );
    movingMeshFileName = dataManager->GetInputFile( "bioid_0001.pts" );
  }
};

TEST_F( ElastixFilterTest, Instantiation )
{
  EXPECT_NO_THROW( ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New() );
}

TEST_F( ElastixFilterTest, PairwiseRegistration )
{
  ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();

  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImage ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImage ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  // TODO: This update should not be needed
  EXPECT_NO_THROW( elastixFilter->Update() );

  EXPECT_NO_THROW( resultImage = elastixFilter->GetOutput() );
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( "ElastixResultImage.nii" );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

}

TEST_F( ElastixFilterTest, MultiPairwiseRegistration )
{
  // TODO: Fix "error while setting up xout" when running standalone binary
  ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();

  DataObjectContainerPointer fixedImages = DataObjectContainerType::New();
  fixedImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( fixedImage );
  fixedImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( fixedImage );

  DataObjectContainerPointer movingImages = DataObjectContainerType::New();
  movingImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( movingImage );
  movingImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( movingImage );

  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImages ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImages ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  // TODO: This update should not be needed
  EXPECT_NO_THROW( elastixFilter->Update() );
  EXPECT_NO_THROW( resultImage = elastixFilter->GetOutput() );
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( "ElastixResultImage.nii" );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );
}

TEST_F( ElastixFilterTest, PointSetRegistration )
{
  ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();

  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImage ) );
  EXPECT_NO_THROW( elastixFilter->SetFixedMeshFileName( fixedMeshFileName  ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImage ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingMeshFileName( movingMeshFileName ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( correspondingPointsParameterObject ) );

  // TODO: This update should not be needed
  EXPECT_NO_THROW( elastixFilter->Update() );

  EXPECT_NO_THROW( resultImage = elastixFilter->GetOutput() );
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( "ElastixResultImage.nii" );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

}

// TODO: Write test with point sets
