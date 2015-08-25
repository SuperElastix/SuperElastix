#include "itkImage.h"
#include "itkImageFileReader.h"
#include "elxDataManager.h"
#include "gtest/gtest.h"

#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"

class itkRegistration : public ::testing::Test {
public:
  virtual void SetUp()
  {
    // TODO: Loading images here result in segfault
  }

  typedef itk::Image< unsigned short, 2 > ImageType;

  typedef itk::ImageFileReader< ImageType > FixedImageReaderType;
  typedef itk::ImageFileReader< ImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer fixedImageReader;
  MovingImageReaderType::Pointer movingImageReader;

};

TEST_F( itkRegistration, ImageRegistration3 )
{ 

  typedef itk::TranslationTransform< double, ImageType::ImageDimension > TransformType;
  typedef itk::RegularStepGradientDescentOptimizer                       OptimizerType;
  typedef itk::LinearInterpolateImageFunction< ImageType, double >       InterpolatorType;
  typedef itk::ImageRegistrationMethod< ImageType, ImageType >           RegistrationType;


  typedef itk::MeanSquaresImageToImageMetric< ImageType, ImageType > MetricType;

  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  MetricType::Pointer         metric        = MetricType::New();
  registration->SetMetric( metric  );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );

  FixedImageReaderType::Pointer fixedImageReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  DataManager::Pointer dataManager = DataManager::New();

  fixedImageReader->SetFileName( dataManager->GetInputFullPath( "BrainProtonDensitySlice.png" ) );
  movingImageReader->SetFileName( dataManager->GetInputFullPath( "BrainProtonDensitySliceR10X13Y17.png" ) );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );

  fixedImageReader->Update(); // This is needed to make the BufferedRegion valid.
  registration->SetFixedImageRegion( fixedImageReader->GetOutput()->GetBufferedRegion() );

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );

  initialParameters[0] = 0.0;  // Initial offset in mm along X
  initialParameters[1] = 0.0;  // Initial offset in mm along Y

  registration->SetInitialTransformParameters( initialParameters );

  optimizer->SetMaximumStepLength( 4.00 );  
  optimizer->SetMinimumStepLength( 0.01 );
  optimizer->SetNumberOfIterations( 200 );

  optimizer->MaximizeOff();

  EXPECT_NO_THROW( registration->Update() );
  RecordProperty( "MetricValue", optimizer->GetValue() );
}

