/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxSuperElastixFilterCustomComponents.h"

#include "selxNiftyregReadImageComponent.h"
#include "selxNiftyregWriteImageComponent.h"
#include "selxItkToNiftiImageSourceComponent.h"
#include "selxNiftiToItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingComparisonImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "selxFunctor.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxDisplacementFieldImageWarperComponent.h"
#include "selxItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"
#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class NiftyregComponentTest : public ::testing::Test
{
public:

  typedef Blueprint::Pointer BlueprintPointer;
  typedef Blueprint::ParameterMapType ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef DataManager DataManagerType;

  /** register all example components */
  typedef TypeList< Niftyregf3dComponent< float >,
    NiftyregReadImageComponent< float >,
    NiftyregWriteImageComponent< float >,
    ItkToNiftiImageSourceComponent< 2, float >,
    NiftiToItkImageSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkToNiftiImageSourceComponent< 3, float >,
    NiftiToItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 3, float >,
    NiftyregSplineToDisplacementFieldComponent< float>,
    DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
    NiftyregAladinComponent< float >,
    ItkDisplacementFieldImageWarperComponent<2, float, float>,
	ItkDisplacementFieldSourceComponent<2, float>,
	ItkImageSinkComponent<2, float > > RegisterComponents;

  typedef SuperElastixFilterCustomComponents< RegisterComponents > SuperElastixFilterType;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New();
    dataManager = DataManagerType::New();
    logger = Logger::New();
    logger->AddStream("cout", std::cout);
    logger->SetLogLevel(LogLevel::TRC);
  }



  virtual void TearDown()
  {
    // Unregister all components after each test
    itk::ObjectFactoryBase::UnRegisterAllFactories();
    // Delete the SuperElastixFilter after each test
    superElastixFilter = nullptr;
  }

  BlueprintPointer blueprint;
  Logger::Pointer logger;
  SuperElastixFilterBase::Pointer superElastixFilter;
  DataManagerType::Pointer dataManager;
};

TEST_F( NiftyregComponentTest, Register2d_nifti )
{
  // Test Niftyreg using Niftyreg's own file readers and writers (NiftyregReadImageComponent, NiftyregWriteImageComponent are for testing purposes only since they do not implement proper SuperElastix Sinks and Sources). 
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetInputFile( "r16slice.nii.gz" ) } } } );
  blueprint->SetComponent( "MovingImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetInputFile( "r64slice.nii.gz" ) } } } );
  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile( "Niftireg_warped_r64to16.nii.gz" ) } } } );

  blueprint->SetConnection( "FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { {} } ); //{ { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));
  EXPECT_NO_THROW( superElastixFilter->Update() );
}
TEST_F( NiftyregComponentTest, ItkToNiftiImage )
{
  // Test for Source component ItkToNiftiImage
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile( "ItkToNiftiImage_converted.nii.gz" ) } } } );

  blueprint->SetConnection( "FixedImage", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } } );

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  // Set up the readers and writers
  auto fixedImageReader = itk::ImageFileReader< itk::Image< float, 3 >>::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImage", fixedImageReader->GetOutput() );

  //EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, NiftiToItkImage )
{
  // Test for Sink component NiftiToItkImage
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();
  // TODO proper 3d nii.gz input data
  //blueprint->SetComponent("FixedImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetInputFile("r16slice.nii.gz") } } });
  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile( "ItkToNiftiImage_converted.nii.gz" ) } } } );
  blueprint->SetComponent( "ResultDomainImage", { { "NameOfClass", { "ItkImageSourceComponent" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } } );

  blueprint->SetConnection( "FixedImage", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } } );
  blueprint->SetConnection( "ResultDomainImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } );

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  // Set up the readers and writers
  auto fixedDomainImageReader = itk::ImageFileReader< itk::Image< float, 3 >>::New();
  fixedDomainImageReader->SetFileName( this->dataManager->GetOutputFile( "ItkToNiftiImage_converted.nii.gz" ) );

  superElastixFilter->SetInput( "ResultDomainImage", fixedDomainImageReader->GetOutput() );

  auto fixedImageWriter = itk::ImageFileWriter< itk::Image< float, 3 >>::New();
  fixedImageWriter->SetFileName( dataManager->GetOutputFile( "NiftiToItkImage_converted.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  fixedImageWriter->SetInput( superElastixFilter->GetOutput< itk::Image< float, 3 >>( "ResultImage" ) );

  fixedImageWriter->Update();
}

TEST_F( NiftyregComponentTest, Register2d_itkImages )
{
  // Test Niftyreg component with proper sink and source.
  /** make example blueprint configuration */
  // Set max iterations to 1 for speedy tests.
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } },
                                                   { "Metric", { "LNCC" } },
                                                   { "NumberOfResolutions", { "1" } },
                                                   { "NumberOfIterations", { "1" } },
                                                   { "Optimizer", { "Gradient" } },
                                                   { "GridSpacingInVoxels", { "8", "8" } } } );

  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );

  blueprint->SetConnection( "FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );
  blueprint->SetConnection( "FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } );
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } } );

  blueprint->Write( dataManager->GetOutputFile( "Register2d_itkImages.dot" ) );

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "r16slice.nii.gz") );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "r64slice.nii.gz" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "Niftireg_warped_r64to16_itkconverted.nii.gz" ) );

  //DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  //resultDisplacementWriter->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImage", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImage", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImage" ) );
  //resultDisplacementWriter->SetInput(superElastixFilter->GetOutput< DisplacementImage2DType >("ResultDisplacementFieldSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
}

TEST_F( NiftyregComponentTest, WBIRDemo )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } } );
  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "ResultDisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );
  blueprint->SetConnection( "FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } );
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "ResultDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "ResultDisplacementField", { {} });

  blueprint->Write( dataManager->GetOutputFile( "Niftyreg_WBIR.dot" ) );

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;
  typedef itk::Image< itk::Vector<float,2>, 2 >              DisplacementImage2DType;
  typedef itk::ImageFileWriter< DisplacementImage2DType > DisplacementImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "Niftyreg_WBIR_Image.mhd" ) );

  DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  resultDisplacementWriter->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImage", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImage", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImage" ) );

  resultDisplacementWriter->SetInput(superElastixFilter->GetOutput< DisplacementImage2DType >("ResultDisplacementField"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
  EXPECT_NO_THROW(resultDisplacementWriter->Update());
}
TEST_F(NiftyregComponentTest, WarpByItkDisplacement)
{
	//Compare niftyreg warped image with image warped by itk. 
	//Warped image and displacement field are created by WBIRDemo, which is required to have run.
	//TODO: make this an independent test. To run everything in one blueprint, currently an extra component is needed that can convert niftyreg bspline image to itk displacement field (and not pass as sink). Or, if passed to sink this test should have two chained superelastices.
	BlueprintPointer blueprint = Blueprint::New();
	blueprint->MergeFromFile(dataManager->GetConfigurationFile("warp_by_displacement.json"));
	BlueprintPointer blueprint_dim = Blueprint::New();
	blueprint_dim->SetComponent("DisplacementFieldImageWarper", { { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
	blueprint->ComposeWith(blueprint_dim);

	superElastixFilter->SetLogger(logger);
	superElastixFilter->SetBlueprint(blueprint);

	auto movingImageReader = superElastixFilter->GetInputFileReader("MovingImage");
	superElastixFilter->SetInput("MovingImage",movingImageReader->GetOutput());
	movingImageReader->SetFileName(dataManager->GetInputFile("coneB2d64.mhd"));

	auto displacementReader = superElastixFilter->GetInputFileReader("DisplacementField");
	superElastixFilter->SetInput("DisplacementField", displacementReader->GetOutput());
	displacementReader->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd")); //displacement was output of WBIRDemo

	auto warpedImageWriter = superElastixFilter->GetOutputFileWriter("WarpedImage");
	warpedImageWriter->SetInput(superElastixFilter->GetOutput("WarpedImage"));
	warpedImageWriter->SetFileName(this->dataManager->GetOutputFile("Niftyreg_WBIR_Image_warpedbyitk.mhd"));
	warpedImageWriter->Update();
	using itkWarpedImageType = itk::Image<float,2>;

	auto niftyreg_warped_image_reader = itk::ImageFileReader<itkWarpedImageType>::New();
	// read Output of WBIRDemo, but Niftyreg writes nans for background and itk doesn't
	niftyreg_warped_image_reader->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Image.mhd"));

	// find all nans in the image
	using Mask2DType = itk::Image<unsigned char, 2>;
	using NanToMaskFilter = itk::UnaryFunctorImageFilter<itkWarpedImageType, Mask2DType, Functor::IsNan<float>>;

	auto nanToMaskFilter = NanToMaskFilter::New();
	nanToMaskFilter->SetInput(niftyreg_warped_image_reader->GetOutput());
	auto nanMask = nanToMaskFilter->GetOutput();
	auto statisticsFilter = itk::StatisticsImageFilter<Mask2DType>::New();
	statisticsFilter->SetInput(nanMask);
	statisticsFilter->GetSumOutput()->Update();
	const auto numberOfNans = statisticsFilter->GetSumOutput()->Get(); 
	// The amount of background nans is 87. For flexibility of the test we expect less than 100.
	EXPECT_LT(numberOfNans,100);

	// set values of both image to zero at the nan positions
    auto maskNegatedImageFilter1 = itk::MaskNegatedImageFilter<itkWarpedImageType, Mask2DType, itkWarpedImageType>::New();
	maskNegatedImageFilter1->SetMaskImage(nanMask);
	maskNegatedImageFilter1->SetInput(niftyreg_warped_image_reader->GetOutput());

	auto maskNegatedImageFilter2 = itk::MaskNegatedImageFilter<itkWarpedImageType, Mask2DType, itkWarpedImageType>::New();
	maskNegatedImageFilter2->SetMaskImage(nanMask);
	maskNegatedImageFilter2->SetInput(superElastixFilter->GetOutput<itkWarpedImageType>("WarpedImage"));

	auto diff = itk::Testing::ComparisonImageFilter<itkWarpedImageType, itkWarpedImageType>::New();
	diff->SetTestInput(maskNegatedImageFilter1->GetOutput());
	diff->SetValidInput(maskNegatedImageFilter2->GetOutput());
	diff->SetDifferenceThreshold(0.0);
	//diff->SetToleranceRadius(radiusTolerance);
	diff->UpdateLargestPossibleRegion();
	bool differenceFailed = false;
	const double averageIntensityDifference = diff->GetTotalDifference();
	// Niftyreg_WBIR_Image_warpedbyitk.mhd seems to have some quantization errors but total difference is less that 96.15;
	EXPECT_LT(averageIntensityDifference, 100);

}

TEST_F( NiftyregComponentTest, AladinWBIRDemo )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "NiftyregAladinComponent" } } } );
  blueprint->SetComponent( "FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );
  blueprint->SetComponent( "ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } } );

  blueprint->SetConnection( "FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );
  blueprint->SetConnection( "FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } );
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } } );

  blueprint->Write( dataManager->GetOutputFile( "NiftyregAladin_WBIR.dot" ) );

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregAladin_WBIR_Image.mhd" ) );

  //DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  //resultDisplacementWriter->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImage", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImage", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImage" ) );
  //resultDisplacementWriter->SetInput(superElastixFilter->GetOutput< DisplacementImage2DType >("ResultDisplacementFieldSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));
  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
}

TEST_F(NiftyregComponentTest, AffineAndBSpline)
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod1", { { "NameOfClass", { "NiftyregAladinComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetComponent("RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("RegistrationMethod1", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } });

  blueprint->SetConnection("RegistrationMethod2", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });


  blueprint->Write(dataManager->GetOutputFile("NiftyregAffineAndBspline.dot"));

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("coneB2d64.mhd"));

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("NiftyregAffineAndBspline_Image.mhd"));

  //DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  //resultDisplacementWriter->SetFileName(dataManager->GetOutputFile("Niftyreg_WBIR_Displacement.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< Image2DType >("ResultImage"));
  //resultDisplacementWriter->SetInput(superElastixFilter->GetOutput< DisplacementImage2DType >("ResultDisplacementFieldSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
}
TEST_F(NiftyregComponentTest, DISABLED_niftyreg_vs_itk_warped)
{
	// Compare 3 warped images: ResultImageNifti ResultImageITKConverted and ResultImageITKWarped
	/** make example blueprint configuration */
	BlueprintPointer blueprint = Blueprint::New();
	blueprint->SetComponent("FixedImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetInputFile("r16slice.nii.gz") } } });
	blueprint->SetComponent("MovingImage", { { "NameOfClass", { "NiftyregReadImageComponent" } }, { "FileName", { this->dataManager->GetInputFile("r64slice.nii.gz") } } });
	blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
	// Test: compare warped
	blueprint->SetComponent("ResultImageNifti", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("niftyreg_vs_itk_warped_ResultImageNifti.nii.gz") } } });
	blueprint->SetComponent("ResultImageITKConverted", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
	//blueprint->SetComponent("TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });

	blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
	blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
	blueprint->SetConnection("RegistrationMethod", "ResultImageNifti", { {} }); //{ { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });
	blueprint->SetConnection("RegistrationMethod", "ResultImageITKConverted", { {} }); //{ { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });
	//blueprint->SetConnection("RegistrationMethod", "TransformToDisplacementField", { {} });

	EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
	EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));
	auto ResultImageITKWriter = superElastixFilter->GetOutputFileWriter("ResultImageITKConverted");
	ResultImageITKWriter->SetInput(superElastixFilter->GetOutput("ResultImageITKConverted"));
	ResultImageITKWriter->SetFileName(this->dataManager->GetOutputFile("niftyreg_vs_itk_warped_ResultImageITKConverted.nii.gz"));
	ResultImageITKWriter->Update();

	//EXPECT_NO_THROW(superElastixFilter->Update());
}

}
