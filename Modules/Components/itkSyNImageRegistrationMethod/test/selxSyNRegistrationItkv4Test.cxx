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

#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxItkDisplacementFieldSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "selxItkImageSinkComponent.h"

#include "selxItkSyNImageRegistrationMethodComponent.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component.h"
#include "selxItkMeanSquaresImageToImageMetricv4Component.h"
#include "selxItkGradientDescentOptimizerv4Component.h"
#include "selxItkAffineTransformComponent.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformComponent.h"
#include "selxItkTransformDisplacementFilterComponent.h"
#include "selxItkResampleFilterComponent.h"
#include "selxItkTransformSinkComponent.h"
#include "selxItkTransformSourceComponent.h"

#include "selxItkCompositeTransformComponent.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

#include "itkTransformFileWriter.h"
#include "itkTransformFileReader.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class SyNRegistrationItkv4Test : public ::testing::Test
{
public:

  /** register all example components */
  typedef TypeList< 
    GDOptimizer3rdPartyComponent,
    GDOptimizer4thPartyComponent,
    SSDMetric3rdPartyComponent,
    SSDMetric4thPartyComponent,
    ItkDisplacementFieldSinkComponent< 3, double >,
    ItkDisplacementFieldSinkComponent< 2, float >,
    ItkImageSinkComponent< 3, double >,
    ItkImageSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkImageSourceComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 2, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, float >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 2, float >,
    ItkSyNImageRegistrationMethodComponent< 3, float, double >,
    ItkSyNImageRegistrationMethodComponent< 3, double, double >,
    ItkSyNImageRegistrationMethodComponent< 2, float, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, double, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float, double >,
    ItkAffineTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkTransformDisplacementFilterComponent< 3, float, double >,
    ItkTransformDisplacementFilterComponent< 2, float, double >,
    ItkTransformDisplacementFilterComponent< 3, double, double >,
    ItkResampleFilterComponent< 2, float, double >,
    ItkResampleFilterComponent< 3, double, double >> RegisterComponents;

  typedef Blueprint::Pointer BlueprintPointer;

  typedef SuperElastixFilterBase::Pointer       SuperElastixFilterBlueprintPointer;
  typedef SuperElastixFilterBase::ConstPointer  SuperElastixFilterBlueprintConstPointer;

  typedef BlueprintImpl::ParameterMapType       ParameterMapType;
  typedef BlueprintImpl::ParameterValueType     ParameterValueType;
  typedef DataManager                           DataManagerType;

  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  typedef itk::Image< double, 3 >             Image3DType;
  typedef itk::ImageFileReader< Image3DType > ImageReader3DType;
  typedef itk::ImageFileWriter< Image3DType > ImageWriter3DType;

  typedef itk::Image< itk::Vector< float, 2 >, 2 >        DisplacementImage2DType;
  typedef itk::ImageFileWriter< DisplacementImage2DType > DisplacementImageWriter2DType;

  typedef itk::Image< itk::Vector< double, 3 >, 3 >       DisplacementImage3DType;
  typedef itk::ImageFileWriter< DisplacementImage3DType > DisplacementImageWriter3DType;

  typedef itk::Transform<double, 3, 3> Transform3DNakedType;
  typedef itk::DataObjectDecorator<itk::Transform<double,3,3>> Transform3DType;
  typedef itk::TransformFileWriterTemplate<double> TransformWriterType;
  typedef itk::TransformFileReaderTemplate<double> TransformReaderType;
  
  Logger::Pointer logger;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New();
    dataManager        = DataManagerType::New();
    logger             = Logger::New();
    logger->AddStream( "cout", std::cout );
    logger->SetLogLevel( LogLevel::TRC );
  }


  virtual void TearDown()
  {
    // Delete the SuperElastixFilter after each test
    superElastixFilter = nullptr;
  }


  BlueprintPointer blueprint;
  SuperElastixFilterBase::Pointer superElastixFilter;
  DataManagerType::Pointer dataManager;
};

TEST_F(SyNRegistrationItkv4Test, FullyConfigured3d)
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  //blueprint->SetLogger( logger );

  // Components
  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "ItkSyNImageRegistrationMethodComponent" } },
  { "Dimensionality", { "3" } }, {"ShrinkFactorsPerLevel", { "4", "2", "1" }}, { "SmoothingSigmasPerLevel", { "4", "2", "1" } } });
 
  blueprint->SetComponent("FixedImageSource", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } } });

  blueprint->SetComponent("MovingImageSource", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "3" } } });

  blueprint->SetComponent("ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "3" } } });

  blueprint->SetComponent("ResultDisplacementFieldSink", { { "NameOfClass", { "ItkDisplacementFieldSinkComponent" } }, { "Dimensionality", { "3" } } });

  blueprint->SetComponent("Metric", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } }, { "Dimensionality", { "3" } } });

  blueprint->SetComponent("TransformDisplacementFilter", { { "NameOfClass", { "ItkTransformDisplacementFilterComponent" } }, { "Dimensionality", { "3" } } });
 blueprint->SetComponent("ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } },
  { "Dimensionality", { "3" } } });

  // Connections
  blueprint->SetConnection("FixedImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageFixedInterface" } } });

  blueprint->SetConnection("MovingImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageMovingInterface" } } });

  blueprint->SetConnection("ResampleFilter", "ResultImageSink", { { "NameOfInterface", { "itkImageInterface" } } });

  blueprint->SetConnection("TransformDisplacementFilter", "ResultDisplacementFieldSink", { { "NameOfInterface", { "itkDisplacementFieldInterface" } } });
  blueprint->SetConnection("Metric", "RegistrationMethod", { { "NameOfInterface", { "itkMetricv4Interface" } } });

  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->Write( dataManager->GetOutputFile( "SyNRegistrationItkv4Test_FullyConfigured3d.dot" ) );

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "SyNRegistrationItkv4Test_FullyConfigured3d_DisplacementField_image.mhd" ) );

  DisplacementImageWriter3DType::Pointer resultDisplacementWriter = DisplacementImageWriter3DType::New();
  resultDisplacementWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_DisplacementField_displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image3DType >( "ResultImageSink" ) );
  resultDisplacementWriter->SetInput( superElastixFilter->GetOutput< DisplacementImage3DType >( "ResultDisplacementFieldSink" ) );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );
  EXPECT_NO_THROW( superElastixFilter->SetLogger( logger ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( resultDisplacementWriter->Update() );
}
TEST_F( SyNRegistrationItkv4Test, WBIRDemo )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("RegistrationMethod", { { "NameOfClass", { "ItkSyNImageRegistrationMethodComponent" } }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("Metric", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } }, { "Dimensionality", { "2" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkImageSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } }, { "Dimensionality", { "2" } } } );
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("TransformToDisplacementField", { { "NameOfClass", { "ItkTransformDisplacementFilterComponent" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultDisplacementField", { { "NameOfClass", { "ItkDisplacementFieldSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod", { { "NameOfInterface", { "itkImageFixedInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod", { { "NameOfInterface", { "itkImageMovingInterface" } } });
  blueprint->SetConnection("Metric", "RegistrationMethod", { { "NameOfInterface", { "itkMetricv4Interface" } } });

  blueprint->SetConnection("ResampleFilter", "ResultImage", { { "NameOfInterface", { "itkImageInterface" } } });
  blueprint->SetConnection("RegistrationMethod", "ResampleFilter", { {} });
  blueprint->SetConnection("RegistrationMethod", "TransformToDisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "TransformToDisplacementField", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("TransformToDisplacementField", "ResultDisplacementField", { {} });
  blueprint->SetConnection("FixedImage", "ResampleFilter", { { "NameOfInterface", { "itkImageDomainFixedInterface" } } });
  blueprint->SetConnection("MovingImage", "ResampleFilter", { { "NameOfInterface", { "itkImageMovingInterface" } } });

  blueprint->Write( dataManager->GetOutputFile( "SyN_ANTSCC.dot" ) );

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "SyN_ANTSCC_Image.mhd" ) );

  DisplacementImageWriter2DType::Pointer resultDisplacementWriter = DisplacementImageWriter2DType::New();
  resultDisplacementWriter->SetFileName( dataManager->GetOutputFile( "SyN_ANTSCC_Displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImage", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImage", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImage" ) );
  resultDisplacementWriter->SetInput( superElastixFilter->GetOutput< DisplacementImage2DType >( "ResultDisplacementField" ) );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( resultDisplacementWriter->Update() );
}
} // namespace selx
