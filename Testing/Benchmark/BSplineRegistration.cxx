/**
 * Benchmarking multi-resolution non-rigid b-spline registration 
 * with mutual information in ITK, Elastix and ANTs
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// DeformableRegistration1
#include "itkRescaleIntensityImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkFEMRegistrationFilter.h"

#include "gtest/gtest.h"

class itkExamples : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        const char *fixedImageName, *movingImageName;
        if ( argc < 2 )
        {
          std::cout << "Image filenames missing." << std::endl;
          std::cout << "Usage: " << argv[0] << " fixedImageFilename movingImageFilename"
                    << std::endl;
          return EXIT_FAILURE;
        }
        else
        {
          fixedImageFilename = argv[1];
          movingImageFilename = argv[2];
        }
    }

};

TEST_F( BSplineRegistration, ITK ) {

  const unsigned int ImageDimension = 2;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, ImageDimension > ImageType;

  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;
  
  // Transform
  typedef itk::BSplineDeformableTransform< CoordinateRepType,
                                           SpaceDimension,
                                           SplineOrder > TransformType;

  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;
  RegionType::SizeType  gridSizeOnImage;
  RegionType::SizeType  gridBorderSize;
  RegionType::SizeType  totalGridSize;
 
  gridSizeOnImage.Fill( 5 );
  gridBorderSize.Fill( 3 ); // Border for spline order = 3 ( 1 lower, 2 upper )
  totalGridSize = gridSizeOnImage + gridBorderSize;
 
  bsplineRegion.SetSize( totalGridSize );
 
  typedef TransformType::SpacingType SpacingType;
  SpacingType spacing = fixedImage->GetSpacing();
 
  typedef TransformType::OriginType OriginType;
  OriginType origin = fixedImage->GetOrigin();
 
  ImageType::SizeType fixedImageSize = fixedRegion.GetSize();
 
  for(unsigned int r=0; r<ImageDimension; r++)
  {
    spacing[r] *= static_cast<double>(fixedImageSize[r] - 1)  /
                  static_cast<double>(gridSizeOnImage[r] - 1);
  }
 
  ImageType::DirectionType gridDirection = fixedImage->GetDirection();
  SpacingType gridOriginOffset = gridDirection * spacing;
 
  OriginType gridOrigin = origin - gridOriginOffset;
 
  transform->SetGridSpacing( spacing );
  transform->SetGridOrigin( gridOrigin );
  transform->SetGridRegion( bsplineRegion );
  transform->SetGridDirection( gridDirection );

  typedef TransformType::ParametersType ParametersType;
  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  ParametersType parameters( numberOfParameters );
  parameters.Fill( 0.0 );
  transform->SetParameters( parameters );

  // Optimizer
  typedef itk::LBFGSOptimizer OptimizerType;

  optimizer->SetGradientConvergenceTolerance( 0.05 );
  optimizer->SetLineSearchAccuracy( 0.9 );
  optimizer->SetDefaultStepLength( .5 );
  optimizer->TraceOn();
  optimizer->SetMaximumNumberOfFunctionEvaluations( 1024 );
 
  // Metric
  typedef itk::MeanSquaresImageToImageMetric< ImageType,
                                              ImageType > MetricType;

  // Interpolator
  typedef itk:: LinearInterpolateImageFunction< ImageType,
                                                double >  InterpolatorType;

  // Setup the registration
  typedef itk::ImageRegistrationMethod< ImageType,
                                        ImageType > RegistrationType;

  RegistrationType::Pointer registration  = RegistrationType::New();
  registration->SetNumberOfThreads( 1 );
  registration->SetMetric( MetricType::New() );
  registration->SetOptimizer( OptimizerType::New() );
  registration->SetInterpolator( InterpolatorType::New() );
 
  TransformType::Pointer transform = TransformType::New();
  registration->SetTransform( transform );

  registration->SetFixedImage( fixedImage  );
  registration->SetMovingImage(  movingImage );
 
  ImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedRegion );
 
  registration->SetInitialTransformParameters( transform->GetParameters() );

  // Execute
  EXPECT_NO_THROW( registration->Update() );

}