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

#include "selxSuperElastixFilter.h"

#include "selxTransformComponent1.h"
#include "selxMetricComponent1.h"
#include "selxGDOptimizer3rdPartyComponent.h"
#include "selxGDOptimizer4thPartyComponent.h"
#include "selxSSDMetric3rdPartyComponent.h"
#include "selxSSDMetric4thPartyComponent.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"
#include "selxItkImageSink.h"

#include "selxItkSyNImageRegistrationMethodComponent.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkGradientDescentOptimizerv4.h"
#include "selxItkAffineTransform.h"
#include "selxItkGaussianExponentialDiffeomorphicTransform.h"
#include "selxItkTransformDisplacementFilter.h"
#include "selxItkResampleFilter.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxRegistrationController.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

#include "selxDefaultComponents.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class SyNRegistrationItkv4Test : public ::testing::Test
{
public:

  typedef std::shared_ptr< Blueprint >                       BlueprintPointer;
  typedef itk::SharedPointerDataObjectDecorator< Blueprint > BlueprintITKType;
  typedef BlueprintITKType::Pointer                          BlueprintITKPointer;
  typedef Blueprint::ParameterMapType                        ParameterMapType;
  typedef Blueprint::ParameterValueType                      ParameterValueType;
  typedef DataManager                                        DataManagerType;

  /** register all example components */
  typedef TypeList< TransformComponent1,
    MetricComponent1,
    GDOptimizer3rdPartyComponent,
    GDOptimizer4thPartyComponent,
    SSDMetric3rdPartyComponent,
    SSDMetric4thPartyComponent,
    DisplacementFieldItkImageFilterSinkComponent< 3, double >,
    DisplacementFieldItkImageFilterSinkComponent< 2, float >,
    ItkImageSinkComponent< 3, double >,
    ItkImageSinkComponent< 2, float >,
    ItkImageSourceFixedComponent< 2, float >,
    ItkImageSourceMovingComponent< 2, float >,
    ItkImageSourceFixedComponent< 3, double >,
    ItkImageSourceMovingComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 2, double >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 3, float >,
    ItkSmoothingRecursiveGaussianImageFilterComponent< 2, float >,
    ItkSyNImageRegistrationMethodComponent< 3, double >,
    ItkSyNImageRegistrationMethodComponent< 2, float >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, double, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float, double >,
    ItkAffineTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkTransformDisplacementFilterComponent< 2, float, double >,
    ItkTransformDisplacementFilterComponent< 3, double, double >,
    ItkResampleFilterComponent< 2, float, double >,
    ItkResampleFilterComponent< 3, double, double >,
    RegistrationControllerComponent< >> RegisterComponents;

  typedef SuperElastixFilter< RegisterComponents > SuperElastixFilterType;

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

  virtual void SetUp()
  {
  }


  virtual void TearDown()
  {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }


  BlueprintPointer blueprint;
};

TEST_F( SyNRegistrationItkv4Test, FullyConfigured3d )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  ParameterMapType component0Parameters;
  component0Parameters[ "NameOfClass" ]    = { "ItkSyNImageRegistrationMethodComponent" };
  component0Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "RegistrationMethod", component0Parameters );

  ParameterMapType component1Parameters;
  component1Parameters[ "NameOfClass" ]    = { "ItkImageSourceFixedComponent" };
  component1Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "FixedImageSource", component1Parameters );

  ParameterMapType component2Parameters;
  component2Parameters[ "NameOfClass" ]    = { "ItkImageSourceMovingComponent" };
  component2Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "MovingImageSource", component2Parameters );

  ParameterMapType component3Parameters;
  component3Parameters[ "NameOfClass" ]    = { "ItkImageSinkComponent" };
  component3Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "ResultImageSink", component3Parameters );

  ParameterMapType component4Parameters;
  component4Parameters[ "NameOfClass" ]    = { "DisplacementFieldItkImageFilterSinkComponent" };
  component4Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "ResultDisplacementFieldSink", component4Parameters );

  ParameterMapType component5Parameters;
  component5Parameters[ "NameOfClass" ]    = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component5Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "Metric", component5Parameters );

  ParameterMapType component6Parameters;
  component6Parameters[ "NameOfClass" ]    = { "ItkTransformDisplacementFilterComponent" };
  component6Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "TransformDisplacementFilter", component6Parameters );

  blueprint->SetComponent( "ResampleFilter", { { "NameOfClass",    { "ItkResampleFilterComponent" } },
                                               { "Dimensionality", { "3" } } } );

  blueprint->SetComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "ResampleFilter", "ResultImageSink", connection3Parameters );

  ParameterMapType connection4Parameters;
  connection4Parameters[ "NameOfInterface" ] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->SetConnection( "TransformDisplacementFilter", "ResultDisplacementFieldSink", connection4Parameters );

  ParameterMapType connection5Parameters;
  connection5Parameters[ "NameOfInterface" ] = { "itkMetricv4Interface" };
  blueprint->SetConnection( "Metric", "RegistrationMethod", connection5Parameters );

  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->SetConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->SetConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->SetConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface
  blueprint->SetConnection( "ResultImageSink", "Controller", { {} } );             //AfterRegistrationInterface
  blueprint->SetConnection( "ResultDisplacementFieldSink", "Controller", { {} } ); //AfterRegistrationInterface

  // Instantiate SuperElastix
  SuperElastixFilterType::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "SyNRegistrationItkv4Test_DisplacementField_image.mhd" ) );

  DisplacementImageWriter3DType::Pointer resultDisplacementWriter = DisplacementImageWriter3DType::New();
  resultDisplacementWriter->SetFileName( dataManager->GetOutputFile( "SyNRegistrationItkv4Test_DisplacementField_displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image3DType >( "ResultImageSink" ) );
  resultDisplacementWriter->SetInput( superElastixFilter->GetOutput< DisplacementImage3DType >( "ResultDisplacementFieldSink" ) );

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( resultDisplacementWriter->Update() );

  blueprint->Write( dataManager->GetOutputFile( "SyNRegistrationItkv4Test_DisplacementField_network.dot" ) );
}
TEST_F( SyNRegistrationItkv4Test, WBIRDemo )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  ParameterMapType component0Parameters;
  component0Parameters[ "NameOfClass" ]    = { "ItkSyNImageRegistrationMethodComponent" };
  component0Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the inputs
  blueprint->SetComponent( "RegistrationMethod", component0Parameters );

  ParameterMapType component1Parameters;
  component1Parameters[ "NameOfClass" ]    = { "ItkImageSourceFixedComponent" };
  component1Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the inputs
  blueprint->SetComponent( "FixedImageSource", component1Parameters );

  ParameterMapType component2Parameters;
  component2Parameters[ "NameOfClass" ]    = { "ItkImageSourceMovingComponent" };
  component2Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the inputs
  blueprint->SetComponent( "MovingImageSource", component2Parameters );

  ParameterMapType component3Parameters;
  component3Parameters[ "NameOfClass" ]    = { "ItkImageSinkComponent" };
  component3Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the outputs
  blueprint->SetComponent( "ResultImageSink", component3Parameters );

  ParameterMapType component4Parameters;
  component4Parameters[ "NameOfClass" ]    = { "DisplacementFieldItkImageFilterSinkComponent" };
  component4Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the outputs
  blueprint->SetComponent( "ResultDisplacementFieldSink", component4Parameters );

  ParameterMapType component5Parameters;
  component5Parameters[ "NameOfClass" ]    = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component5Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the inputs
  blueprint->SetComponent( "Metric", component5Parameters );

  ParameterMapType component6Parameters;
  component6Parameters[ "NameOfClass" ]    = { "ItkTransformDisplacementFilterComponent" };
  component6Parameters[ "Dimensionality" ] = { "2" }; // should be derived from the inputs
  blueprint->SetComponent( "TransformDisplacementFilter", component6Parameters );

  blueprint->SetComponent( "ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } },
                                               { "Dimensionality", { "2" } } } );

  blueprint->SetComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "ResampleFilter", "ResultImageSink", connection3Parameters );

  ParameterMapType connection4Parameters;
  connection4Parameters[ "NameOfInterface" ] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->SetConnection( "TransformDisplacementFilter", "ResultDisplacementFieldSink", connection4Parameters );

  ParameterMapType connection5Parameters;
  connection5Parameters[ "NameOfInterface" ] = { "itkMetricv4Interface" };
  blueprint->SetConnection( "Metric", "RegistrationMethod", connection5Parameters );

  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->SetConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->SetConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->SetConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface
  blueprint->SetConnection( "ResultImageSink", "Controller", { {} } );             //AfterRegistrationInterface
  blueprint->SetConnection( "ResultDisplacementFieldSink", "Controller", { {} } ); //AfterRegistrationInterface

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  blueprint->Write( dataManager->GetOutputFile( "SyN_ANTSCC.dot" ) );

  // Instantiate SuperElastix
  SuperElastixFilterType::Pointer superElastixFilter;
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

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
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  resultDisplacementWriter->SetInput( superElastixFilter->GetOutput< DisplacementImage2DType >( "ResultDisplacementFieldSink" ) );

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( resultDisplacementWriter->Update() );
}
} // namespace selx
