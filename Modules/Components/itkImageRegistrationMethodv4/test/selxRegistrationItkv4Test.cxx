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

#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkGradientDescentOptimizerv4.h"
#include "selxItkAffineTransform.h"
#include "selxItkGaussianExponentialDiffeomorphicTransform.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.h"
#include "selxItkTransformDisplacementFilter.h"
#include "selxItkResampleFilter.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxRegistrationController.h"
#include "selxItkCompositeTransformComponent.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"

#include "selxDefaultComponents.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class RegistrationItkv4Test : public ::testing::Test
{
public:

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
    ItkImageRegistrationMethodv4Component< 3, double, double >,
    ItkImageRegistrationMethodv4Component< 2, float, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, double, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float, double  >,
    ItkGradientDescentOptimizerv4Component< double >,
    ItkAffineTransformComponent< double, 3 >,
    ItkAffineTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 3, double >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
    ItkTransformDisplacementFilterComponent< 2, float, double >,
    ItkTransformDisplacementFilterComponent< 3, double, double >,
    ItkResampleFilterComponent< 2, float, double >,
    ItkResampleFilterComponent< 3, double, double >,
    RegistrationControllerComponent< >,
    ItkCompositeTransformComponent<double, 3>,
    ItkCompositeTransformComponent<double, 2 >> RegisterComponents;

  typedef std::unique_ptr< Blueprint >                        BlueprintPointer;
  typedef itk::UniquePointerDataObjectDecorator< Blueprint >  BlueprintITKType;
  typedef BlueprintITKType::Pointer                           BlueprintITKPointer;

  typedef SuperElastixFilter::BlueprintType SuperElastixFilterBlueprintType;
  typedef SuperElastixFilter::Pointer       SuperElastixFilterBlueprintPointer;
  typedef SuperElastixFilter::ConstPointer  SuperElastixFilterBlueprintConstPointer;
  typedef Blueprint::ParameterMapType           ParameterMapType;
  typedef Blueprint::ParameterValueType         ParameterValueType;
  typedef DataManager                           DataManagerType;

  typedef itk::Image< float, 2 >                Image2DType;
  typedef itk::ImageFileReader< Image2DType >   ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType >   ImageWriter2DType;

  typedef itk::Image< double, 3 >               Image3DType;
  typedef itk::ImageFileReader< Image3DType >   ImageReader3DType;
  typedef itk::ImageFileWriter< Image3DType >   ImageWriter3DType;

  typedef itk::Image< itk::Vector< double, 3 >, 3 >       DisplacementImage3DType;
  typedef itk::ImageFileWriter< DisplacementImage3DType > DisplacementImageWriter3DType;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New();
    dataManager = DataManagerType::New();
  }

  virtual void TearDown()
  {
    // Unregister all components after each test 
    itk::ObjectFactoryBase::UnRegisterAllFactories();
    // Delete the SuperElastixFilter after each test 
    superElastixFilter = nullptr;
  }

  // Blueprint holds a configuration for SuperElastix
  BlueprintPointer blueprint;
  SuperElastixFilter::Pointer superElastixFilter;
  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager;
};

// These test are disabled, since may not want to check each component with this much boilerplate code.
// We should consider moving functional tests outside the unit tests
// Anyway, the Blueprint configuration needs to be updated
TEST_F( RegistrationItkv4Test, DISABLED_3DImagesOnly )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  ParameterMapType component0Parameters;
  component0Parameters[ "NameOfClass" ]    = { "ItkImageRegistrationMethodv4Component" };
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

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "RegistrationMethod", "ResultImageSink", connection3Parameters );

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_ImagesOnly.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image3DType >( "ResultImageSink" ) );

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
}

// These test are disabled, since may not want to check each component with this much boilerplate code.
// We should consider moving functional tests outside the unit tests
// Anyway, the Blueprint configuration needs to be updated
TEST_F( RegistrationItkv4Test, DISABLED_3DANTSCCMetric )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  ParameterMapType component0Parameters;
  component0Parameters[ "NameOfClass" ]    = { "ItkImageRegistrationMethodv4Component" };
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
  component4Parameters[ "NameOfClass" ]    = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  component4Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "Metric", component4Parameters );

  ParameterMapType component5Parameters;
  component5Parameters[ "NameOfClass" ]        = { "ItkGradientDescentOptimizerv4Component" };
  component5Parameters[ "NumberOfIterations" ] = { "1" };
  blueprint->SetComponent( "Optimizer", component5Parameters );

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "RegistrationMethod", "ResultImageSink", connection3Parameters );

  ParameterMapType connection4Parameters;
  connection4Parameters[ "NameOfInterface" ] = { "itkMetricv4Interface" };
  blueprint->SetConnection( "Metric", "RegistrationMethod", connection4Parameters );

  ParameterMapType connection5Parameters;
  connection5Parameters[ "NameOfInterface" ] = { "itkOptimizerv4Interface" };
  blueprint->SetConnection( "Optimizer", "RegistrationMethod", connection5Parameters );


  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_WithANTSCCMetric.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image3DType >( "ResultImageSink" ) );

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
}

// These test are disabled, since may not want to check each component with this much boilerplate code.
// We should consider moving functional tests outside the unit tests
// Anyway, the Blueprint configuration needs to be updated
TEST_F( RegistrationItkv4Test, DISABLED_3DMeanSquaresMetric )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  ParameterMapType component0Parameters;
  component0Parameters[ "NameOfClass" ]    = { "ItkImageRegistrationMethodv4Component" };
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
  component4Parameters[ "NameOfClass" ]    = { "ItkMeanSquaresImageToImageMetricv4Component" };
  component4Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the inputs
  blueprint->SetComponent( "Metric", component4Parameters );

  ParameterMapType component5Parameters;
  component5Parameters[ "NameOfClass" ]        = { "ItkGradientDescentOptimizerv4Component" };
  component5Parameters[ "NumberOfIterations" ] = { "1" };
  blueprint->SetComponent( "Optimizer", component5Parameters );

  ParameterMapType connection1Parameters;
  connection1Parameters[ "NameOfInterface" ] = { "itkImageFixedInterface" };
  blueprint->SetConnection( "FixedImageSource", "RegistrationMethod", connection1Parameters );

  ParameterMapType connection2Parameters;
  connection2Parameters[ "NameOfInterface" ] = { "itkImageMovingInterface" };
  blueprint->SetConnection( "MovingImageSource", "RegistrationMethod", connection2Parameters );

  ParameterMapType connection3Parameters;
  connection3Parameters[ "NameOfInterface" ] = { "itkImageInterface" };
  blueprint->SetConnection( "RegistrationMethod", "ResultImageSink", connection3Parameters );

  ParameterMapType connection4Parameters;
  connection4Parameters[ "NameOfInterface" ] = { "itkMetricv4Interface" };
  blueprint->SetConnection( "Metric", "RegistrationMethod", connection4Parameters );

  ParameterMapType connection5Parameters;
  connection5Parameters[ "NameOfInterface" ] = { "itkOptimizerv4Interface" };
  blueprint->SetConnection( "Optimizer", "RegistrationMethod", connection5Parameters );

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_WithMeanSquaresMetric.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );
  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image3DType >( "ResultImageSink" ) );

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
}

TEST_F( RegistrationItkv4Test, FullyConfigured3d )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
                                                   { "Dimensionality", { "3" } },
                                                   { "NumberOfLevels", { "2" } },
                                                   { "ShrinkFactorsPerLevel", { "2", "1" } } } );

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
  component6Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "TransformDisplacementFilter", component6Parameters );

  ParameterMapType component7Parameters;
  component7Parameters[ "NameOfClass" ]        = { "ItkGradientDescentOptimizerv4Component" };
  component7Parameters[ "NumberOfIterations" ] = { "1" };
  blueprint->SetComponent( "Optimizer", component7Parameters );

  blueprint->SetComponent( "ResampleFilter", { { "NameOfClass",    { "ItkResampleFilterComponent" } },
                                               { "Dimensionality", { "3" } } } );

  blueprint->SetComponent( "Transform",      { { "NameOfClass",    { "ItkGaussianExponentialDiffeomorphicTransformComponent" } },
                                               { "Dimensionality", { "3" } } } );

  blueprint->SetComponent( "TransformResolutionAdaptor", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
                                                           { "Dimensionality", { "3" } },
                                                           { "ShrinkFactorsPerLevel", { "2", "1" } } } );

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

  blueprint->SetConnection( "FixedImageSource", "Transform", { {} } );
  blueprint->SetConnection( "Transform", "RegistrationMethod", { {} } );

  blueprint->SetConnection( "FixedImageSource", "TransformResolutionAdaptor", { {} } );
  blueprint->SetConnection( "TransformResolutionAdaptor", "RegistrationMethod", { {} } );
  blueprint->SetConnection( "Optimizer", "RegistrationMethod", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->SetConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->SetConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->SetConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface

  blueprint->Write(dataManager->GetOutputFile("RegistrationItkv4Test_DisplacementField_network.dot"));

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_DisplacementField_image.mhd" ) );

  DisplacementImageWriter3DType::Pointer resultDisplacementWriter = DisplacementImageWriter3DType::New();
  resultDisplacementWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_DisplacementField_displacement.mhd" ) );

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

}
TEST_F( RegistrationItkv4Test, FullyConfigured3dAffine )
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent( "RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
                                                   { "Dimensionality", { "3" } },
                                                   { "NumberOfLevels", { "2" } } } );

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
  component6Parameters[ "Dimensionality" ] = { "3" }; // should be derived from the outputs
  blueprint->SetComponent( "TransformDisplacementFilter", component6Parameters );

  ParameterMapType component7Parameters;
  component7Parameters[ "NameOfClass" ]        = { "ItkGradientDescentOptimizerv4Component" };
  component7Parameters[ "NumberOfIterations" ] = { "10" };
  blueprint->SetComponent( "Optimizer", component7Parameters );

  blueprint->SetComponent( "ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } },
                                               { "Dimensionality", { "3" } } } );

  blueprint->SetComponent( "Transform", { { "NameOfClass", { "ItkAffineTransformComponent" } },
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

  //blueprint->AddConnection( "FixedImageSource", "Transform", { {} } );
  blueprint->SetConnection( "Transform", "RegistrationMethod", { {} } );

  blueprint->SetConnection( "Optimizer", "RegistrationMethod", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->SetConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->SetConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->SetConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->SetConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->SetConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->SetConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface

  blueprint->Write(dataManager->GetOutputFile("RegistrationItkv4Test_DisplacementField_network.dot"));

  // Set up the readers and writers
  ImageReader3DType::Pointer fixedImageReader = ImageReader3DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "sphereA3d.mhd" ) );

  ImageReader3DType::Pointer movingImageReader = ImageReader3DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "sphereB3d.mhd" ) );

  ImageWriter3DType::Pointer resultImageWriter = ImageWriter3DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_DisplacementField_image.mhd" ) );

  DisplacementImageWriter3DType::Pointer resultDisplacementWriter = DisplacementImageWriter3DType::New();
  resultDisplacementWriter->SetFileName( dataManager->GetOutputFile( "RegistrationItkv4Test_DisplacementField_displacement.mhd" ) );

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
}

TEST_F(RegistrationItkv4Test, CompositeTransform)
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = BlueprintPointer( new Blueprint() );

  blueprint->SetComponent("MultiStageTransformController", { { "NameOfClass", { "ItkCompositeTransformComponent" } }, { "ExecutionOrder", { "RegistrationMethod1", "RegistrationMethod2" } } });

  blueprint->SetComponent("FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } } });
  blueprint->SetComponent("MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } } });
  blueprint->SetComponent("ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } } });

  blueprint->SetComponent("ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } } });
  blueprint->SetConnection("FixedImageSource", "ResampleFilter", { {} });
  blueprint->SetConnection("MovingImageSource", "ResampleFilter", { {} });
  blueprint->SetConnection("MultiStageTransformController", "ResampleFilter", { {} }); //ReconnectTransformInterface

  blueprint->SetConnection("ResampleFilter", "ResultImageSink", {});


  blueprint->SetComponent("RegistrationMethod1", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
  { "Dimensionality", { "2" } },
  { "InternalComputationValueType", { "double" } },
  { "PixelType", { "float" } } });
  blueprint->SetConnection("FixedImageSource", "RegistrationMethod1", {});
  blueprint->SetConnection("MovingImageSource", "RegistrationMethod1", {});

  blueprint->SetComponent("Metric1", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } } });
  blueprint->SetConnection("Metric1", "RegistrationMethod1", {});
  blueprint->SetComponent("Transform1", { { "NameOfClass", { "ItkAffineTransformComponent" } } });
  blueprint->SetConnection("Transform1", "RegistrationMethod1", {});
  blueprint->SetComponent("Optimizer1", { { "NameOfClass", { "ItkGradientDescentOptimizerv4Component" } } });
  blueprint->SetConnection("Optimizer1", "RegistrationMethod1", {});

  blueprint->SetConnection("RegistrationMethod1", "MultiStageTransformController", {}); // MultiStageTransformInterface

  blueprint->SetComponent("RegistrationMethod2", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
  { "Dimensionality", { "2" } },
  { "InternalComputationValueType", { "double" } },
  { "PixelType", { "float" } },
  { "NumberOfLevels", { "2" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } },
  { "SmoothingSigmasPerLevel", { "2", "1" } } });
  blueprint->SetConnection("FixedImageSource", "RegistrationMethod2", {});
  blueprint->SetConnection("MovingImageSource", "RegistrationMethod2", {});

  blueprint->SetComponent("Metric2", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } } });
  blueprint->SetConnection("Metric2", "RegistrationMethod2", {});
  blueprint->SetComponent("Transform2", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } } });
  blueprint->SetConnection("Transform2", "RegistrationMethod2", {});
  blueprint->SetConnection("FixedImageSource", "Transform2", {});
  blueprint->SetComponent("TransformResolutionAdaptor2", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } } });
  blueprint->SetConnection("FixedImageSource", "TransformResolutionAdaptor2", {});
  blueprint->SetConnection("TransformResolutionAdaptor2", "RegistrationMethod2", {});

  blueprint->SetComponent("Optimizer2", { { "NameOfClass", { "ItkGradientDescentOptimizerv4Component" } } });
  blueprint->SetConnection("Optimizer2", "RegistrationMethod2", {});
  blueprint->SetConnection("RegistrationMethod2", "MultiStageTransformController", {}); // MultiStageTransformInterface

  blueprint->Write(dataManager->GetOutputFile("RegistrationItkv4Test_CompositeTransform.dot"));

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("coneB2d64.mhd"));

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("RegistrationItkv4Test_CompositeTransform.mhd"));

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput("FixedImageSource", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImageSource", movingImageReader->GetOutput());

  resultImageWriter->SetInput(superElastixFilter->GetOutput< Image2DType >("ResultImageSink"));

  BlueprintITKPointer superElastixFilterBlueprint = BlueprintITKType::New();
  superElastixFilterBlueprint->Set( blueprint );
  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( superElastixFilterBlueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW(resultImageWriter->Update());
}
} // namespace selx
