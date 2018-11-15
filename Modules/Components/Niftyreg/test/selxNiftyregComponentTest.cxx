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
#include "itkMaskImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxDisplacementFieldImageWarperComponent.h"
#include "selxItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"
#include "selxNiftyregf3dComponent.h"
#include "selxMonolithicElastixComponent.h"
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
  typedef TypeList<
    Niftyregf3dComponent< float >,
    NiftyregReadImageComponent< float >,
    NiftyregWriteImageComponent< float >,
    ItkToNiftiImageSourceComponent< 2, float >,
    ItkToNiftiImageSourceComponent< 3, float >,
    NiftiToItkImageSinkComponent< 2, float >,
    NiftiToItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 2, float >,
    ItkImageSourceComponent< 3, float >,
    NiftyregSplineToDisplacementFieldComponent< float>,
    DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
    DisplacementFieldNiftiToItkImageSinkComponent< 3, float>,
    NiftyregAladinComponent< float >,
    ItkDisplacementFieldImageWarperComponent<2, float, float>,
    ItkDisplacementFieldImageWarperComponent<3, float, float>,
	  ItkDisplacementFieldSourceComponent<2, float>,
    ItkDisplacementFieldSourceComponent<3, float>,
	  ItkImageSinkComponent<2, float >,
    ItkImageSinkComponent<3, float >
  > RegisterComponents;

  typedef SuperElastixFilterCustomComponents< RegisterComponents > SuperElastixFilterType;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterType::New();
    dataManager = DataManagerType::New();
    logger = Logger::New();
    logger->AddStream("cout", std::cout);
    logger->SetLogLevel(LogLevel::DBG);
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
  EXPECT_NO_THROW( superElastixFilter->SetLogger(logger) );
  EXPECT_NO_THROW( superElastixFilter->Update() );
}

TEST_F( NiftyregComponentTest, DISABLED_Register2d_aladin_anisotropic) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("RegistrationMethod", {{"NameOfClass", {"NiftyregAladinComponent"} } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("Register2d_aladin_scaling_image.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "2" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square2dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square2dAnisotropic.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare2dAnisotropicDeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register2d_aladin_anisotropic_origin) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "NiftyregAladinComponent" } }, { "NumberOfResolutions", { "8" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("Register2d_aladin_scaling_image.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "2" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square2dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square2dAnisotropicOriginX8OriginY12.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare2dAnisotropicOriginX8OriginY12DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register2d_aladin_anisotropoic_translation_origin) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("RegistrationMethod", {{"NameOfClass", {"NiftyregAladinComponent"} } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("Register2d_aladin_scaling_image.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "2" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square2dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 2>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square2dAnisotropicX8Y16OriginX8OriginY12.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare2dAnisotropicX8Y16OriginX8OriginY12DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_origin) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", { {"NameOfClass", {"NiftyregAladinComponent"} }, {"NumberOfResolutions", {"1"} } });
  blueprint->SetComponent("AladinResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegAladinSquare3dOrigin.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "AladinResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("f3dResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegf3dSquare3dOrigin.nii.gz") } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "RegistrationMethod2", "f3dResultImage", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dIsotropicOriginX8OriginY12OriginZ16.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));



  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dIsotropicOriginX8OriginY12OriginZ16DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();
  superElastixFilter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                           { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyRegAladinSquare3dIsotropicOriginX8OriginY12OriginZ16DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dIsotropicOriginX8OriginY12OriginZ16TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();

}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_spacing_both) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", { {"NameOfClass", {"NiftyregAladinComponent"} }, {"NumberOfResolutions", {"3"} } });
  blueprint->SetComponent("AladinResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegAladinSquare3dBothSpacingX2SpacingY2SpacingZ2.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "AladinResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("f3dResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegf3dSquare3dBothSpacingX2SpacingY2SpacingZ2.nii.gz") } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "RegistrationMethod2", "f3dResultImage", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropicSpacingX2SpacingY2SpacingZ2.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dIsotropicSpacingX2SpacingY2SpacingZ2.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dIsotropicBothSpacingX2SpacingY2SpacingZ2DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_spacing_moving) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", { {"NameOfClass", {"NiftyregAladinComponent"} }, {"NumberOfResolutions", {"3"} } });
  blueprint->SetComponent("AladinResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegAladinSquare3dMovingSpacingX1.1SpacingY1.1SpacingZ1.1.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "AladinResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("f3dResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegf3dSquare3dMovingSpacingX1.1SpacingY1.1SpacingZ1.1.nii.gz") } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "RegistrationMethod2", "f3dResultImage", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dIsotropicSpacingX1.1SpacingY1.1SpacingZ1.1.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dIsotropicMovingSpacingX1.1SpacingY1.1SpacingZ1.1DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyRegAladinSquare3dIsotropicMovingSpacingX1.1SpacingY1.1SpacingZ1.1DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dIsotropicMovingSpacingX1.1SpacingY1.1SpacingZ1.1TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_anisotropic) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", {{"NameOfClass", {"NiftyregAladinComponent"} } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("square3dAnisotropic.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dAnisotropic.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dAnisotropicDeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_anisotropic_origin) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "NiftyregAladinComponent" } }, { "NumberOfResolutions", { "8" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("NiftyRegAladinSquare3dAnisotropicOriginX8OriginY12.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dAnisotropicOriginX8OriginY12OriginZ16.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dAnisotropicOriginX8OriginY12OriginZ16DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyRegAladinSquare3dAnisotropicOriginX8OriginY12OriginZ16DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dAnisotropicOriginX8OriginY12OriginZ16TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F( NiftyregComponentTest, DISABLED_Register3d_aladin_anisotropoic_translation_origin) {
  BlueprintPointer blueprint = Blueprint::New();
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass",    { "ItkToNiftiImageSourceComponent" }}, {"PixelType", {"float" } }, { "Dimensionality", { "3" } } });
  blueprint->SetComponent("RegistrationMethod", {{"NameOfClass", {"NiftyregAladinComponent"} } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftyregWriteImageComponent" } }, { "FileName", { this->dataManager->GetOutputFile("square3dAnisotropicX8Y16OriginX8OriginY12.nii.gz") } } });
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregReferenceImageInterface"} } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { {"NameOfInterface", {"NiftyregFloatingImageInterface"} } });
  blueprint->SetConnection( "RegistrationMethod", "ResultImage", { { } } );

  // The aladin component cannot produce a displacement field by itself, so we pass it through the niftyreg
  // bspline component with 0 iterations.
  blueprint->SetComponent( "RegistrationMethod2", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "0" } } } );
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregAffineMatrixInterface" } } } );
  blueprint->SetConnection( "FixedImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } } );
  blueprint->SetConnection( "MovingImage", "RegistrationMethod2", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } } );

  blueprint->SetConnection( "RegistrationMethod2", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );

  auto imageFileReader0 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader0->SetFileName(this->dataManager->GetInputFile("square3dIsotropic.nii.gz"));
  auto imageFileReader1 = itk::ImageFileReader<itk::Image<float, 3>>::New();
  imageFileReader1->SetFileName(this->dataManager->GetInputFile("square3dAnisotropicX8Y16Z24OriginX8OriginY12OriginZ16.nii.gz"));
  superElastixFilter->SetInput("FixedImage", imageFileReader0->GetOutput());
  superElastixFilter->SetInput("MovingImage", imageFileReader1->GetOutput());

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  writer->SetFileName( this->dataManager->GetOutputFile( "NiftyRegAladinSquare3dAnisotropicX8Y16OriginX8OriginY12DeformationField.nii.gz" ) );
  writer->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );
  writer->Update();

  // EXPECT_NO_THROW(superElastixFilter->Update());
  superElastixFilter->Update();
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
    auto maskImageFilter1 = itk::MaskImageFilter<itkWarpedImageType, Mask2DType, itkWarpedImageType>::New();
	maskImageFilter1->SetMaskImage(nanMask);
	maskImageFilter1->SetInput(niftyreg_warped_image_reader->GetOutput());


	auto maskImageFilter2 = itk::MaskImageFilter<itkWarpedImageType, Mask2DType, itkWarpedImageType>::New();
	maskImageFilter2->SetMaskImage(nanMask);
	maskImageFilter2->SetInput(superElastixFilter->GetOutput<itkWarpedImageType>("WarpedImage"));

	auto diff = itk::Testing::ComparisonImageFilter<itkWarpedImageType, itkWarpedImageType>::New();
	diff->SetTestInput(maskImageFilter1->GetOutput());
	diff->SetValidInput(maskImageFilter2->GetOutput());
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

TEST_F(NiftyregComponentTest, AffineAndBSpline_2d)
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
  resultImageWriter->Update();
  EXPECT_NO_THROW(resultImageWriter->Update());
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dIsotropic.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dAnisotropicX8Y16Z24.nii.gz" ) );

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dAnisotropicX8Y16Y12.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dAnisotropicX8Y16Y12DeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dAnisotropicX8Y16Y12DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dAnisotropicX8Y16Y12TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisize_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISize.nii.gz" ) );
  fixedImageReader->Update();

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeX8Y16Z24.nii.gz" ) );
  movingImageReader->Update();

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISize.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeDeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeDeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeTransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizespacing_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } }, { "NumberOfIterations", { "20" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeSpacing.nii.gz" ) );
  fixedImageReader->Update();

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeSpacingX8Y16Z24.nii.gz" ) );
  movingImageReader->Update();

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacing.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacingDeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  superElastixFilter->Update();
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeSpacingDeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacingTransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizeoriginspacing_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacing.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacingX8Y16Z24.nii.gz" ) );

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacing.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingDeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  superElastixFilter->Update();
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeOriginSpacingDeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingTransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizeorigin_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOrigin.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginX8Y16Z24.nii.gz" ) );

  // ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  // resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOrigin.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  // resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer resultImageWriter = superElastixFilter->GetOutputFileWriter( "ResultImage" );
  resultImageWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOrigin.nii.gz" ) );
  resultImageWriter->SetInput( superElastixFilter->GetOutput( "ResultImage" ) );

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginDeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  superElastixFilter->Update();
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeOriginDeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginTransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizespacingbothtrans_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeSpacing.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeSpacingX8Y16Z24.nii.gz" ) );

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacingBothX8Y16Z24.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacingBothX8Y16Z24DeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeSpacingBothX8Y16Z24DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeSpacingBothX8Y16Z24TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizeoriginspacingtrans_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacing.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacingX8Y16Z24.nii.gz" ) );

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24DeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}

TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popisizeoriginspacingtransdifferentspacings_nii)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacing.nii.gz" ) );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "square3dPOPISizeOriginSpacingX8Y16Z24.nii.gz" ) );
  movingImageReader->Update();

  ImageType::SpacingType spacing;
  spacing[0] = 1.2;
  spacing[1] = 1.2;
  spacing[2] = 1.9;
  movingImageReader->GetOutput()->SetSpacing(spacing);
  movingImageReader->Update();

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24DeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24DeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplineSquare3dPOPISizeOriginSpacingX8Y16Z24TransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}


TEST_F(NiftyregComponentTest, DISABLED_BSpline_3d_popi)
{
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "Niftyregf3dComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageSourceComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "NiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });
  blueprint->SetConnection("FixedImage", "ResultImage", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResultImage", { { "NameOfInterface", { "NiftyregWarpedImageInterface" } } });

  // Write deformation field
  blueprint->SetComponent("DisplacementField", { { { "NameOfClass"}, {"DisplacementFieldNiftiToItkImageSinkComponent"} },
                                                 { { "Dimensionality" }, { "3" } } });
  blueprint->SetComponent( "TransformToDisplacementField", { { "NameOfClass", { "NiftyregSplineToDisplacementFieldComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent( "DisplacementField", { { "NameOfClass", { "DisplacementFieldNiftiToItkImageSinkComponent" } }, { "Dimensionality", { "3" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "TransformToDisplacementField", { {} });
  blueprint->SetConnection( "TransformToDisplacementField", "DisplacementField", { {} });
  blueprint->SetConnection( "FixedImage", "DisplacementField", { { "NameOfInterface", {"itkImageDomainFixedInterface"} } } );


  // Set up the readers and writers
  typedef itk::Image< float, 3 >              ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( "/Users/kasper/Development/SuperBenchData/POPI/dx/mhd/00.mhd" );

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( "/Users/kasper/Development/SuperBenchData/POPI/dx/mhd/50.mhd" );

  ImageWriterType::Pointer resultImageWriter = ImageWriterType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "NiftyregBSplinePOPIdx.nii.gz" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImage", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());
  resultImageWriter->SetInput(superElastixFilter->GetOutput< ImageType >("ResultImage"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));
  EXPECT_NO_THROW(superElastixFilter->SetLogger(logger));

  selx::AnyFileWriter::Pointer deformationFieldWriter = superElastixFilter->GetOutputFileWriter( "DisplacementField" );
  deformationFieldWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplinePOPIdxDeformationField.nii.gz" ) );
  deformationFieldWriter->SetInput( superElastixFilter->GetOutput( "DisplacementField" ) );

  // Update call on the writers triggers SuperElastix to configure and execute
  resultImageWriter->Update();
  deformationFieldWriter->Update();

  // To test that image is warped correctly with transformix, we do it here.
  // We make a separate blueprint and load displacementfield from disk since we cannot get the ITK deformation field
  // from the deformation field sink
  SuperElastixFilterBase::Pointer transformixSuperElastixFilter;
  transformixSuperElastixFilter = SuperElastixFilterType::New();
  BlueprintPointer transformixBlueprint = Blueprint::New();
  transformixBlueprint->SetComponent("DisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "PixelType", {"float"} }, { "Dimensionality", { "3" } } });
  transformixBlueprint->SetComponent("ImageWarper", { { "NameOfClass", { "ItkDisplacementFieldImageWarperComponent" } },
                                                      { "PixelType", {"float"} }, { "Dimensionality", { "3" } } } );
  transformixBlueprint->SetConnection("DisplacementField", "ImageWarper", {});
  transformixBlueprint->SetConnection("MovingImage", "ImageWarper", { { "NameOfInterface", {"itkImageMovingInterface"}}});
  transformixBlueprint->SetConnection("ImageWarper", "ResultImage", {});

  transformixSuperElastixFilter->SetInput("MovingImage", movingImageReader->GetOutput());

  auto imageFileReader2 = itk::ImageFileReader<itk::Image<itk::Vector< float, 3 >, 3>>::New();
  imageFileReader2->SetFileName(this->dataManager->GetOutputFile("NiftyregBSplinePOPIdxDeformationField.nii.gz"));
  transformixSuperElastixFilter->SetInput("DisplacementField", imageFileReader2->GetOutput());
  EXPECT_NO_THROW(transformixSuperElastixFilter->SetBlueprint(transformixBlueprint));

  selx::AnyFileWriter::Pointer transformixWriter = transformixSuperElastixFilter->GetOutputFileWriter( "ResultImage" );
  transformixWriter->SetFileName( this->dataManager->GetOutputFile( "NiftyregBSplinePOPIdxTransformixWarpedImage.nii.gz" ) );
  transformixWriter->SetInput( transformixSuperElastixFilter->GetOutput( "ResultImage" ) );
  transformixWriter->Update();

  transformixSuperElastixFilter->Update();
}


}
