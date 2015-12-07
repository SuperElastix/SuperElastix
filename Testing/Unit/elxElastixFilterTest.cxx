#include "elxElastixFilter.h"
#include "elxParameterObject.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

using namespace selx;

class ElastixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  typedef itk::Image< float, 2u >                       ImageType;
  ImageType::Pointer fixedImage;
  ImageType::Pointer movingImage;
  ImageType::Pointer resultImage;

  typedef itk::ImageFileReader< ImageType >             ImageReaderType;
  typedef itk::ImageFileWriter< ImageType >             ImageWriterType;

  typedef ParameterObject::ParameterValuesType          ParameterValuesType;
  typedef ParameterObject::ParameterMapType             ParameterMapType;
  ParameterObject::Pointer parameterObject;

  typedef ElastixFilter< ImageType, ImageType, ImageType > ElastixFilterType;


  virtual void SetUp()
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReaderType::Pointer reader = ImageReaderType::New();

    reader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    fixedImage = reader->GetOutput();

    // TODO: Only call update on writer
    reader->Update();

    reader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    movingImage = reader->GetOutput();

    // TODO: Only call update on writer
    reader->Update();

    // ParameterMap
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
    parameterMap[ "WriteResultImage" ]                  = ParameterValuesType( 1, "false" );
    parameterMap[ "ResultImageFormat" ]                 = ParameterValuesType( 1, "nii" );

    // Registration
    parameterMap[ "Registration" ]                      = ParameterValuesType( 1, "MultiResolutionRegistration" );
    parameterMap[ "Transform" ]                         = ParameterValuesType( 1, "EulerTransform" );
    parameterMap[ "Metric" ]                            = ParameterValuesType( 1, "AdvancedMattesMutualInformation" );
    parameterMap[ "MaximumNumberOfIterations" ]         = ParameterValuesType( 1, "128" );

    parameterObject = ParameterObject::New();
    parameterObject->SetParameterMap( parameterMap );
  }
};

TEST_F( ElastixFilterTest, Instantiation )
{
  EXPECT_NO_THROW( ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New() );
}

TEST_F( ElastixFilterTest, Registration )
{
  ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();

  elastixFilter->LogToConsoleOn();

  elastixFilter->SetFixedImage( fixedImage );
  elastixFilter->SetMovingImage( movingImage );
  elastixFilter->SetParameterObject( parameterObject );

  // TODO: This update should not be needed
  elastixFilter->Update();

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( "ElastixResultImage.nii" );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

  // TODO: Return transform parameters
}
