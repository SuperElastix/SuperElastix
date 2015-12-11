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
  typedef ParameterObject::ParameterValuesType              ParameterValuesType;
  typedef ParameterObject::ParameterMapType                 ParameterMapType;

  // Elastix typedefs
  typedef ElastixFilter< ImageType, ImageType >             ElastixFilterType;
  typedef ElastixFilterType::DataObjectContainerType        DataObjectContainerType;
  typedef ElastixFilterType::DataObjectContainerPointer     DataObjectContainerPointer;

  // Variables used by multiple tests
  ImageType::Pointer fixedImage;
  ImageType::Pointer movingImage;
  ImageType::Pointer resultImage;

  std::string fixedMeshFileName;
  std::string movingMeshFileName;

  ParameterObject::Pointer parameterObject;
  ParameterObject::Pointer correspondingPointsParameterObject;
  ParameterObject::ConstPointer transformParameterObject;

  virtual void SetUp()
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    // TODO: All calls to update here should not be needed but should be propagated to reader by ElastixFilter

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
    parameterMap[ "FixedInternalImagePixelType" ]       = ParameterValuesType( 1, "float" );
    parameterMap[ "FixedImageDimension" ]               = ParameterValuesType( 1, "2" );
    parameterMap[ "MovingInternalImagePixelType" ]      = ParameterValuesType( 1, "float" );    
    parameterMap[ "MovingImageDimension" ]              = ParameterValuesType( 1, "2" );
    parameterMap[ "ResultImagePixelType" ]              = ParameterValuesType( 1, "float" );

    // Common components
    parameterMap[ "FixedImagePyramid" ]                 = ParameterValuesType( 1, "FixedSmoothingImagePyramid" );
    parameterMap[ "MovingImagePyramid" ]                = ParameterValuesType( 1, "MovingSmoothingImagePyramid" );
    parameterMap[ "Interpolator"]                       = ParameterValuesType( 1, "LinearInterpolator");
    parameterMap[ "Optimizer" ]                         = ParameterValuesType( 1, "AdaptiveStochasticGradientDescent" );
    parameterMap[ "Resampler"]                          = ParameterValuesType( 1, "DefaultResampler" );
    parameterMap[ "ResampleInterpolator"]               = ParameterValuesType( 1, "FinalLinearInterpolator" );
    parameterMap[ "FinalBSplineInterpolationOrder" ]    = ParameterValuesType( 1, "2" );
    parameterMap[ "NumberOfResolutions" ]               = ParameterValuesType( 1, "2" );

    // Image Sampler
    parameterMap[ "ImageSampler" ]                      = ParameterValuesType( 1, "RandomCoordinate" ); 
    parameterMap[ "NumberOfSpatialSamples"]             = ParameterValuesType( 1, "2048" );
    parameterMap[ "CheckNumberOfSamples" ]              = ParameterValuesType( 1, "true" );
    parameterMap[ "MaximumNumberOfSamplingAttempts" ]   = ParameterValuesType( 1, "8" );
    parameterMap[ "NewSamplesEveryIteration" ]          = ParameterValuesType( 1, "true");

    // Optimizer
    parameterMap[ "NumberOfSamplesForExactGradient" ]   = ParameterValuesType( 1, "4096" );
    parameterMap[ "DefaultPixelValue" ]                 = ParameterValuesType( 1, "0" );
    parameterMap[ "AutomaticParameterEstimation" ]      = ParameterValuesType( 1, "true" );

    // Output
    parameterMap[ "WriteResultImage" ]                  = ParameterValuesType( 1, "true" );
    parameterMap[ "ResultImageFormat" ]                 = ParameterValuesType( 1, "nii" );

    // Registration
    parameterMap[ "Registration" ]                      = ParameterValuesType( 1, "MultiResolutionRegistration" );
    parameterMap[ "Transform" ]                         = ParameterValuesType( 1, "EulerTransform" );
    parameterMap[ "Metric" ]                            = ParameterValuesType( 1, "AdvancedMattesMutualInformation" );
    parameterMap[ "MaximumNumberOfIterations" ]         = ParameterValuesType( 1, "128" );

    ParameterMapType nonRigidParameterMap = parameterMap;
    parameterObject = ParameterObject::New();
    parameterObject->SetParameterMap( nonRigidParameterMap );

    ParameterMapType correspondingPointsParameterMap = parameterMap;

    // TODO: WHY DOES THIS SEGFAULT?
    //correspondingPointsParameterMap[ "Metric" ][ 1 ] = "CorrespondingPointsEuclideanDistanceMetric";
    //correspondingPointsParameterObject->SetParameterMap( correspondingPointsParameterMap );

    fixedMeshFileName = dataManager->GetInputFile( "bioid_0000.vtk" );
    movingMeshFileName = dataManager->GetInputFile( "bioid_0001.vtk" );
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

// TODO: Write test with point sets
