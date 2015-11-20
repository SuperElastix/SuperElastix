#include "elxElastixComponent.h"

#include "itkImageFileReader.h"

#include "gtest/gtest.h"
#include "elxDataManager.h"

using namespace selx;

class ElastixComponentTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  typedef itk::Image< float, 2u >                     ImageType;
  typedef itk::ImageFileReader< ImageType >           ImageReaderType;

  ImageType::Pointer fixedImage;
  ImageType::Pointer movingImage;

  typedef ElastixComponent< ImageType >                   ElastixComponentType;
  typedef ElastixComponentType::ParameterMapType          ParameterMapType;
  typedef ElastixComponentType::ParameterMapListType      ParameterMapListType;
  typedef ElastixComponentType::ParameterValuesType       ParameterValuesType;
  typedef ElastixComponentType::DataObjectIdentifierType  DataObjectIdentifierType;

  ParameterMapListType parameterMapList;

  virtual void SetUp()
  {
    DataManagerType::Pointer dataManager = DataManagerType::New();

    ImageReaderType::Pointer reader = ImageReaderType::New();

    reader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    fixedImage = reader->GetOutput();
    reader->Update();

    reader->SetFileName( dataManager->GetInputFile( "cthead1-Float.mha" ) );
    movingImage = reader->GetOutput();
    reader->Update();

    // ParameterMap
    ParameterMapType parameterMap = ParameterMapType();

    // Common components
    parameterMap[ "FixedImagePyramid" ]                 = ParameterValuesType( 1, "FixedSmoothingImagePyramid" );
    parameterMap[ "MovingImagePyramid" ]                = ParameterValuesType( 1, "MovingSmoothingImagePyramid" );
    parameterMap[ "Interpolator"]                       = ParameterValuesType( 1, "LinearInterpolator");
    parameterMap[ "Optimizer" ]                         = ParameterValuesType( 1, "AdaptiveStochasticGradientDescent" );
    parameterMap[ "Resampler"]                          = ParameterValuesType( 1, "DefaultResampler" );
    parameterMap[ "ResampleInterpolator"]               = ParameterValuesType( 1, "FinalBSplineInterpolator" );
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
    parameterMap[ "Registration" ]                    = ParameterValuesType( 1, "MultiResolutionRegistration" );
    parameterMap[ "Transform" ]                       = ParameterValuesType( 1, "EulerTransform" );
    parameterMap[ "Metric" ]                          = ParameterValuesType( 1, "AdvancedMattesMutualInformation" );
    parameterMap[ "MaximumNumberOfIterations" ]       = ParameterValuesType( 1, "128" );

    ParameterMapListType parameterMapList = ParameterMapListType( 1, parameterMap );
  }
};

TEST_F( ElastixComponentTest, Instantiation )
{
  typedef ElastixComponentType::ParameterMapType      ParameterMapType;
  typedef ElastixComponentType::ParameterValuesType   ParameterValuesType;

  ElastixComponentType::Pointer elastixComponent = ElastixComponentType::New();
}

TEST_F( ElastixComponentTest, Registration )
{
  typedef ElastixComponentType::ParameterMapType      ParameterMapType;
  typedef ElastixComponentType::ParameterValuesType   ParameterValuesType;

  ElastixComponentType::Pointer elastixComponent = ElastixComponentType::New();

  elastixComponent->SetInput( DataObjectIdentifierType( "FixedImage" ), fixedImage );
  elastixComponent->SetInput( DataObjectIdentifierType( "MovingImage" ), fixedImage );
  elastixComponent->SetParameterMapList( parameterMapList );
  elastixComponent->Update();
}
