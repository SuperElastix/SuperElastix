#include "elxElastixFilter.h"
#include "elxParameterObject.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

// TODO:
// - In the following examples, why do we need to call update on both reader and elastixFilter
//   before using the writer? Should the call to update on the writer not be propagated upstream?
//   (See http://itk.org/Wiki/ITK/Examples/Segmentation/OtsuThresholdImageFilter which defeats the whole
//   purpose of having a pipeline, no?)
// - Compare result images against baseline
// - SetUp() runs before every test. Does GoogleTest have a function that is called once before tests are run?


using namespace selx;

class ElastixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;

  // Parameter typedefs
  typedef ParameterObject::ParameterVectorType              ParameterVectorType;
  typedef ParameterObject::ParameterMapType                 ParameterMapType;

  ParameterMapType parameterMap;
  ParameterObject::Pointer eulerTransformParameterObject;
  ParameterObject::Pointer affineTransformParameterObject;
  ParameterObject::Pointer correspondingPointsParameterObject;
  ParameterObject::Pointer nonRigidParameterObject;
  ParameterObject::Pointer nonRigidParameterObject3D;
  ParameterObject::Pointer groupwiseParameterObject;
  ParameterObject::ConstPointer transformParameterObject;

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

    ParameterMapType affineTransformParameterMap = parameterMap;
    affineTransformParameterMap[ "Transform" ] = ParameterVectorType( 1, "AffineTransform" );
    affineTransformParameterObject = ParameterObject::New();
    affineTransformParameterObject->SetParameterMap( affineTransformParameterMap );

    ParameterMapType correspondingPointsParameterMap = parameterMap;
    correspondingPointsParameterMap[ "Registration" ] = ParameterVectorType( 1, "MultiMetricMultiResolutionRegistration" );
    correspondingPointsParameterMap[ "Transform" ] = ParameterVectorType( 1, "TranslationTransform" );
    correspondingPointsParameterMap[ "Metric" ].push_back( "CorrespondingPointsEuclideanDistanceMetric" );
    correspondingPointsParameterMap[ "Metric0Weight"] = ParameterVectorType( 1, "0.0" );
    correspondingPointsParameterObject = ParameterObject::New();
    correspondingPointsParameterObject->SetParameterMap( correspondingPointsParameterMap );

    ParameterMapType nonRigidParameterMap = parameterMap;
    nonRigidParameterMap[ "Transform" ] = ParameterVectorType( 1, "BSplineTransform" );
    nonRigidParameterObject = ParameterObject::New();
    nonRigidParameterObject->SetParameterMap( nonRigidParameterMap );

    ParameterMapType nonRigidParameterMap3D = parameterMap;
    nonRigidParameterMap3D[ "FixedInternalImagePixelType" ] = ParameterVectorType( 1, "float" );
    nonRigidParameterMap3D[ "FixedImageDimension" ] = ParameterVectorType( 1, "3" );
    nonRigidParameterMap3D[ "MovingInternalImagePixelType" ] = ParameterVectorType( 1, "float" );
    nonRigidParameterMap3D[ "MovingImageDimension" ] = ParameterVectorType( 1, "3" );
    nonRigidParameterMap3D[ "ResultImagePixelType" ] = ParameterVectorType( 1, "float" );
    nonRigidParameterMap3D[ "Transform" ] = ParameterVectorType( 1, "BSplineTransform" );
    nonRigidParameterMap3D[ "MaximumNumberOfIterations" ] = ParameterVectorType( 1, "8" );
    nonRigidParameterObject3D = ParameterObject::New();
    nonRigidParameterObject3D->SetParameterMap( nonRigidParameterMap3D );

    ParameterMapType groupwiseParameterMap = parameterMap;
    groupwiseParameterMap[ "FixedInternalImagePixelType" ] = ParameterVectorType( 1, "float" );
    groupwiseParameterMap[ "FixedImageDimension" ] = ParameterVectorType( 1, "4" );
    groupwiseParameterMap[ "MovingInternalImagePixelType" ] = ParameterVectorType( 1, "float" );
    groupwiseParameterMap[ "MovingImageDimension" ] = ParameterVectorType( 1, "4" );
    groupwiseParameterMap[ "ResultImagePixelType" ] = ParameterVectorType( 1, "float" );
    groupwiseParameterMap[ "Transform" ] = ParameterVectorType( 1, "BSplineStackTransform" );
    groupwiseParameterMap[ "Metric" ] = ParameterVectorType( 1, "VarianceOverLastDimensionMetric" );
    groupwiseParameterMap[ "MaximumNumberOfIterations" ] = ParameterVectorType( 1, "512" );
    groupwiseParameterMap[ "Interpolator"] = ParameterVectorType( 1, "ReducedDimensionBSplineInterpolator" );
    groupwiseParameterMap[ "ResampleInterpolator" ] = ParameterVectorType( 1, "FinalReducedDimensionBSplineInterpolator" );
    groupwiseParameterMap[ "MaximumNumberOfIterations" ] = ParameterVectorType( 1, "8" );
    groupwiseParameterObject = ParameterObject::New();
    groupwiseParameterObject->SetParameterMap( nonRigidParameterMap );
  }
};

TEST_F( ElastixFilterTest, Instantiation )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  EXPECT_NO_THROW( ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New() );
}

TEST_F( ElastixFilterTest, Euler2D )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( eulerTransformParameterObject ) );

  // TODO: This update should not be needed (see description above)
  EXPECT_NO_THROW( elastixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "Euler2DResultImage.nii" ) );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

TEST_F( ElastixFilterTest, AffineWithMultipleFixedAndMovingImages2D )
 {
   typedef itk::Image< float, 2 > ImageType;
   typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
   typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
   typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
   typedef ElastixFilterType::DataObjectContainerType  DataObjectContainerType;
   typedef ElastixFilterType::DataObjectContainerPointer DataObjectContainerPointer;

   DataManagerType::Pointer dataManager = DataManagerType::New();

   ImageFileReaderType::Pointer fixedImageReader0 = ImageFileReaderType::New();
   fixedImageReader0->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
   fixedImageReader0->Update();

   ImageFileReaderType::Pointer fixedImageReader1 = ImageFileReaderType::New();
   fixedImageReader1->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
   fixedImageReader1->Update();

   ImageFileReaderType::Pointer movingImageReader0 = ImageFileReaderType::New();
   movingImageReader0->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17S12.png" ) );
   movingImageReader0->Update();

   ImageFileReaderType::Pointer movingImageReader1 = ImageFileReaderType::New();
   movingImageReader1->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17S12.png" ) );
   movingImageReader1->Update();

   DataObjectContainerPointer fixedImages = DataObjectContainerType::New();
   fixedImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( fixedImageReader0->GetOutput() );
   fixedImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( fixedImageReader1->GetOutput() );

   DataObjectContainerPointer movingImages = DataObjectContainerType::New();
   movingImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( movingImageReader0->GetOutput() );
   movingImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( movingImageReader1->GetOutput() );

   ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();
   EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
   EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImages ) );
   EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImages ) );
   EXPECT_NO_THROW( elastixFilter->SetParameterObject( affineTransformParameterObject ) );

   // TODO: This update should not be needed (see description above)
   EXPECT_NO_THROW( elastixFilter->Update() );

   ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
   EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "AffineWithMultipleFixedAndMovingImages2DResultImage.nii" ) ) );
   EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
   EXPECT_NO_THROW( writer->Update() );

   EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
 }

TEST_F( ElastixFilterTest, TranslationWithPointSets2D )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  // We generate the point sets manually
  std::ofstream fixedMeshFile;
  fixedMeshFile.open( dataManager->GetInputFile( "FixedMesh.pts" ));
  fixedMeshFile << "point\n";
  fixedMeshFile << "1\n";
  fixedMeshFile << "128.0 128.0\n";
  fixedMeshFile.close();

  std::ofstream movingMeshFile;
  movingMeshFile.open( dataManager->GetInputFile( "MovingMesh.pts" ));
  movingMeshFile << "point\n";
  movingMeshFile << "1\n";
  movingMeshFile << "115.0 111.0\n";
  movingMeshFile.close();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceShifted13x17y.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New();
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetFixedMeshFileName( dataManager->GetInputFile( "FixedMesh.pts" )  ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingMeshFileName( dataManager->GetInputFile( "MovingMesh.pts" ) ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( correspondingPointsParameterObject ) );

  // TODO: This update should not be needed (see description above)
  EXPECT_NO_THROW( elastixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( "TranslationWithPointSets2DResultImage.nii" );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
 }

TEST_F( ElastixFilterTest, BSplineWithFixedMask2D )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer fixedMaskReader = ImageFileReaderType::New();
  fixedMaskReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20Mask.png" ) );
  fixedMaskReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetFixedMask( fixedMaskReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( nonRigidParameterObject ) );

  // TODO: This update should not be needed (see description above)
  EXPECT_NO_THROW( elastixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "BSplineWithFixedMask2DResultImage.nii" ) );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

#ifdef SUPERELASTIX_RUN_LONG_TESTS

TEST_F( ElastixFilterTest, BSpline3D )
{
  typedef itk::Image< float, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "OAS1_0001_MR1_mpr-1_anon.nrrd" ) );
  fixedImageReader->Update();

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "OAS1_0002_MR1_mpr-1_anon.nrrd" ) );
  movingImageReader->Update();

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( nonRigidParameterObject3D ) );

  // TODO: This update should not be needed (see description above)
  EXPECT_NO_THROW( elastixFilter->Update() );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  writer->SetFileName( dataManager->GetOutputFile( "BSpline3DResultImage.nii" ) );
  writer->SetInput( elastixFilter->GetOutput() );
  EXPECT_NO_THROW( writer->Update() );

  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

// TODO: Find out why this segfaults
//TEST_F( ElastixFilterTest, BSpline4D )
//{
//  typedef itk::Image< float, 4 > ImageType;
//  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
//  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
//  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
//
//  DataManagerType::Pointer dataManager = DataManagerType::New();
//
//  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
//  fixedImageReader->SetFileName( dataManager->GetInputFile( "4D.nii.gz" ) );
//  fixedImageReader->Update();
//
//  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
//  movingImageReader->SetFileName( dataManager->GetInputFile( "4D.nii.gz" ) );
//  movingImageReader->Update();
//
//  ElastixFilterType::Pointer elastixFilter;
//  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
//  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
//  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
//  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
//  EXPECT_NO_THROW( elastixFilter->SetParameterObject( groupwiseParameterObject ) );
//
//  // TODO: This update should not be needed (see description above)
//  EXPECT_NO_THROW( elastixFilter->Update() );
//
//  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
//  writer->SetFileName( dataManager->GetOutputFile( "BSpline4DResultImage.nii" ) );
//  writer->SetInput( elastixFilter->GetOutput() );
//  EXPECT_NO_THROW( writer->Update() );
//
//  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
//}

#endif // SUPERELASTIX_RUN_LONG_TESTS
