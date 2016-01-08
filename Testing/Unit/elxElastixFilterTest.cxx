#include "elxElastixFilter.h"
#include "elxParameterObject.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "elxDataManager.h"
#include "gtest/gtest.h"

// TODO: Check that desired results are actually obtained, e.g. by comparing result
// images against a baseline. Right now we only check that no errors are thrown.

using namespace elastix;

class ElastixFilterTest : public ::testing::Test
{
protected:

  typedef DataManager DataManagerType;
  typedef ParameterObject::ParameterValueVectorType ParameterValueVectorType;
};

TEST_F( ElastixFilterTest, Instantiation )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  EXPECT_NO_THROW( ElastixFilterType::Pointer elastixFilter = ElastixFilterType::New() );
}

TEST_F( ElastixFilterTest, UpdateOnGetOutputEuler2D )
{
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "rigid" ) );

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  // We try to write the image because simply calling GetOutput() will not result in an
  // error (a pointer is still passed even if elastix has not run, although the pointee is empty)
  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "UpdateOnGetOutputEuler2DResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );
}

TEST_F( ElastixFilterTest, UpdateOnGetTransformParametersEuler2D )
{
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "rigid" ) );

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  ParameterObject::Pointer transformParameters;

  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );
  EXPECT_NO_THROW( transformParameters = elastixFilter->GetTransformParameters() );
  EXPECT_TRUE( transformParameters->GetParameterMap()[ 0 ].size() > 0 );
}

TEST_F( ElastixFilterTest, AffineWithMultipleFixedAndMovingImages2D )
{
  // TODO: Internal logic to automatically broadcast metric, sampler,
  // interpolator and pyramids to match the number of metrics. Can do
  // it in such a way that we are guaranteed to not mess up user settings?
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "affine" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Registration" ] = ParameterValueVectorType( 1, "MultiMetricMultiResolutionRegistration" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Metric" ] = ParameterValueVectorType( 2, parameterObject->GetParameterMap( 0 )[ "Metric" ][ 0 ] ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "ImageSampler" ] = ParameterValueVectorType( 2, parameterObject->GetParameterMap( 0 )[ "ImageSampler" ][ 0 ] ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Interpolator" ] = ParameterValueVectorType( 2, parameterObject->GetParameterMap( 0 )[ "Interpolator" ][ 0 ] ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "FixedImagePyramid" ] = ParameterValueVectorType( 2, parameterObject->GetParameterMap( 0 )[ "FixedImagePyramid" ][ 0 ] ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "MovingImagePyramid" ] = ParameterValueVectorType( 2, parameterObject->GetParameterMap( 0 )[ "MovingImagePyramid" ][ 0 ] ) );

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;
  typedef ElastixFilterType::DataObjectContainerType  DataObjectContainerType;
  typedef ElastixFilterType::DataObjectContainerPointer DataObjectContainerPointer;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader0 = ImageFileReaderType::New();
  fixedImageReader0->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer fixedImageReader1 = ImageFileReaderType::New();
  fixedImageReader1->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader0 = ImageFileReaderType::New();
  movingImageReader0->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17S12.png" ) );

  ImageFileReaderType::Pointer movingImageReader1 = ImageFileReaderType::New();
  movingImageReader1->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17S12.png" ) );

  // We fill the first pair of fixed and moving images with zeros to be able 
  // to inspect if the subsequent pair is able to drive the registration
  ImageType::Pointer fixedImage0 = fixedImageReader0->GetOutput();
  fixedImage0->Update();
  fixedImage0->DisconnectPipeline();
  fixedImage0->FillBuffer( 0.0 );
  ImageType::Pointer movingImage0 = movingImageReader0->GetOutput();
  movingImage0->Update();
  movingImage0->DisconnectPipeline();
  movingImage0->FillBuffer( 0.0 );

  DataObjectContainerPointer fixedImages;
  EXPECT_NO_THROW( fixedImages = DataObjectContainerType::New() );
  EXPECT_NO_THROW( fixedImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( fixedImage0 ) );
  EXPECT_NO_THROW( fixedImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( fixedImageReader1->GetOutput() ) );

  DataObjectContainerPointer movingImages;
  EXPECT_NO_THROW( movingImages = DataObjectContainerType::New() );
  EXPECT_NO_THROW( movingImages->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( movingImage0 ) );
  EXPECT_NO_THROW( movingImages->CreateElementAt( 1 ) = static_cast< itk::DataObject* >( movingImageReader1->GetOutput() ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImages ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImages ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "AffineWithMultipleFixedAndMovingImages2DResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );

  ParameterObject::Pointer transformParameterObject;
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

TEST_F( ElastixFilterTest, TranslationWithPointSets2D )
{
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "translation" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Registration" ] = ParameterValueVectorType( 1, "MultiMetricMultiResolutionRegistration" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Transform" ] = ParameterValueVectorType( 1, "TranslationTransform" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Metric" ].push_back( "CorrespondingPointsEuclideanDistanceMetric" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "Metric0Weight"] = ParameterValueVectorType( 1, "0.0" ) );

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

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceShifted13x17y.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetFixedMeshFileName( dataManager->GetInputFile( "FixedMesh.pts" )  ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingMeshFileName( dataManager->GetInputFile( "MovingMesh.pts" ) ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( "TranslationWithPointSets2DResultImage.nii" ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );

  ParameterObject::Pointer transformParameterObject;
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
 }

TEST_F( ElastixFilterTest, BSplineWithFixedMask2D )
{
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "nonrigid" ) );

  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer fixedMaskReader = ImageFileReaderType::New();
  fixedMaskReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20Mask.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetFixedMask( fixedMaskReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "BSplineWithFixedMask2DResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );

  ParameterObject::Pointer transformParameterObject;
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

#ifdef SUPERELASTIX_BUILD_LONG_TESTS

TEST_F( ElastixFilterTest, DefaultParameterObject )
{
  typedef itk::Image< float, 2 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceBorder20.png" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "BrainProtonDensitySliceR10X13Y17.png" ) );

  ElastixFilterType::Pointer elastixFilter;

  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  elastixFilter->Update();
}

TEST_F( ElastixFilterTest, BSpline3D )
{
  ParameterObject::Pointer parameterObject;
  EXPECT_NO_THROW( parameterObject = ParameterObject::New() );
  EXPECT_NO_THROW( parameterObject->SetParameterMap( "nonrigid" ) );
  EXPECT_NO_THROW( parameterObject->GetParameterMap( 0 )[ "MaximumNumberOfIterations" ] = ParameterValueVectorType( 1, "8" ) );

  typedef itk::Image< float, 3 > ImageType;
  typedef itk::ImageFileReader< ImageType > ImageFileReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageFileWriterType;
  typedef ElastixFilter< ImageType, ImageType > ElastixFilterType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer fixedImageReader = ImageFileReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "OAS1_0001_MR1_mpr-1_anon.nrrd" ) );

  ImageFileReaderType::Pointer movingImageReader = ImageFileReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "OAS1_0002_MR1_mpr-1_anon.nrrd" ) );

  ElastixFilterType::Pointer elastixFilter;
  EXPECT_NO_THROW( elastixFilter = ElastixFilterType::New() );
  EXPECT_NO_THROW( elastixFilter->LogToConsoleOn() );
  EXPECT_NO_THROW( elastixFilter->SetFixedImage( fixedImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetMovingImage( movingImageReader->GetOutput() ) );
  EXPECT_NO_THROW( elastixFilter->SetParameterObject( parameterObject ) );

  ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
  EXPECT_NO_THROW( writer->SetFileName( dataManager->GetOutputFile( "BSpline3DResultImage.nii" ) ) );
  EXPECT_NO_THROW( writer->SetInput( elastixFilter->GetOutput() ) );
  EXPECT_NO_THROW( writer->Update() );

  ParameterObject::Pointer transformParameterObject;
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

#include "itkCastImageFilter.h" 
TEST_F( ElastixFilterTest, BSpline4D )
{
  ParameterObject::Pointer parameterObject = ParameterObject::New();
  parameterObject->SetParameterMap( "groupwise" );
  parameterObject->GetParameterMap( 0 )[ "MaximumNumberOfIterations" ] = ParameterValueVectorType( 1, "4" );

  typedef itk::Image< float, 4 > FloatImageType;
  typedef itk::ImageFileReader< FloatImageType > ImageFileReaderType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageFileReaderType::Pointer imageReader = ImageFileReaderType::New();
  imageReader->SetFileName( dataManager->GetInputFile( "4D.nii.gz" ) );

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
  EXPECT_NO_THROW( transformParameterObject = elastixFilter->GetTransformParameters() );
}

#endif // SUPERELASTIX_BUILD_LONG_TESTS
