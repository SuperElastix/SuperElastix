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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"

#include "selxElastixComponent.h"
#include "selxMonolithicElastix.h"
#include "selxMonolithicTransformix.h"

#include "selxItkImageSink.h"

#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkGradientDescentOptimizerv4.h"
#include "selxItkGaussianExponentialDiffeomorphicTransform.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.h"
#include "selxItkTransformDisplacementFilter.h"
#include "selxItkResampleFilter.h"
#include "selxRegistrationController.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxDefaultComponents.h"

#include "itkTestingComparisonImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkVectorMagnitudeImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace selx
{
/** A demo for our WBIR paper written as a Unit Test in the Google Test Framework */
class WBIRDemoTest : public ::testing::Test
{
public:

  /** Fill SUPERelastix' component data base by registering various components */
  typedef TypeList<
    DisplacementFieldItkImageFilterSinkComponent< 2, float >,
    ItkImageSinkComponent< 2, float >,
    ItkImageSourceFixedComponent< 2, float >,
    ItkImageSourceMovingComponent< 2, float >,
    ElastixComponent< 2, float >,
    MonolithicElastixComponent< 2, float >,
    MonolithicTransformixComponent< 2, float >,
    ItkImageRegistrationMethodv4Component< 2, float >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float >,
    ItkGradientDescentOptimizerv4Component< double >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
    ItkTransformDisplacementFilterComponent< 2, float, double >,
    ItkResampleFilterComponent< 2, float, double >,
    RegistrationControllerComponent< >> RegisterComponents;

  typedef SuperElastixFilter< RegisterComponents > SuperElastixFilterType;

  typedef Blueprint::Pointer            BlueprintPointerType;
  typedef Blueprint::ConstPointer       BlueprintConstPointerType;
  typedef Blueprint::ParameterMapType   ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef DataManager                   DataManagerType;

  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  typedef itk::Image< itk::Vector< float, 2 >, 2 >  VectorImage2DType;
  typedef itk::ImageFileReader< VectorImage2DType > VectorImageReader2DType;
  typedef itk::ImageFileWriter< VectorImage2DType > VectorImageWriter2DType;

  typedef itk::Testing::ComparisonImageFilter< Image2DType, Image2DType > ComparisonImageFilterType;
  // Unfortunately comparing Vector Images cannot by done by this filter, therefore we define our own comparison pipeline.
  // typedef itk::Testing::ComparisonImageFilter<VectorImage2DType, VectorImage2DType> ComparisonVectorImageFilterType;
  //typedef itk::VectorImageToImageAdaptor<float, 2> VectorImageToImageAdaptorType;
  typedef itk::SubtractImageFilter< VectorImage2DType, VectorImage2DType, VectorImage2DType > SubtractVectorImageFilterType;
  typedef itk::VectorMagnitudeImageFilter< VectorImage2DType, Image2DType >                   VectorMagnitudeImageFilterType;
  typedef itk::StatisticsImageFilter< Image2DType >                                           StatisticsImageFilterType;

  virtual void SetUp()
  {
    baselineImageReader = ImageReader2DType::New();
    compareImageFilter  = ComparisonImageFilterType::New();
    compareImageFilter->SetValidInput( baselineImageReader->GetOutput() );
    compareImageFilter->SetDifferenceThreshold( 0.0 );

    baselineVectorImageReader = VectorImageReader2DType::New();
    subtractVectorImageFilter = SubtractVectorImageFilterType::New();
    VectorMagnitudeImageFilterType::Pointer vectorMagnitudeImageFilter = VectorMagnitudeImageFilterType::New();
    statisticsImageFilter = StatisticsImageFilterType::New();

    subtractVectorImageFilter->SetInput1( baselineVectorImageReader->GetOutput() );

    vectorMagnitudeImageFilter->SetInput( subtractVectorImageFilter->GetOutput() );
    statisticsImageFilter->SetInput( vectorMagnitudeImageFilter->GetOutput() );
  }


  virtual void TearDown()
  {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }


  BlueprintPointerType            blueprint;
  SuperElastixFilterType::Pointer superElastixFilter;

  ImageReader2DType::Pointer             baselineImageReader;
  ComparisonImageFilterType::Pointer     compareImageFilter;
  VectorImageReader2DType::Pointer       baselineVectorImageReader;
  SubtractVectorImageFilterType::Pointer subtractVectorImageFilter;
  StatisticsImageFilterType::Pointer     statisticsImageFilter;
};

/** Experiment 2a: ITKv4 framework, stationary velocity field transform, ANTs neighborhood correlation metric */
TEST_F( WBIRDemoTest, itkv4_SVF_ANTSCC )
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();
  blueprint->AddComponent( "RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
                                                   { "NumberOfLevels", { "3" } },
                                                   { "ShrinkFactorsPerLevel", { "4", "2", "1" } },
                                                   { "SmoothingSigmasPerLevel", { "4", "2", "1" } } } );

  blueprint->AddComponent( "Metric", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } } } );
  blueprint->AddComponent( "Optimizer", { { "NameOfClass", { "ItkGradientDescentOptimizerv4Component" } },
                                          { "NumberOfIterations", { "100" } },
                                          { "LearningRate", { "100" } } } );
  blueprint->AddComponent( "Transform", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } } } );

  blueprint->AddComponent( "TransformResolutionAdaptor", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
                                                           { "ShrinkFactorsPerLevel", { "4", "2", "1" } } } );

  blueprint->AddComponent( "ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } } } );
  blueprint->AddComponent( "TransformDisplacementFilter", { { "NameOfClass", { "ItkTransformDisplacementFilterComponent" } } } );

  blueprint->AddComponent( "FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } } } );
  blueprint->AddComponent( "MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } } } );
  blueprint->AddComponent( "ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } } } );
  blueprint->AddComponent( "ResultDisplacementFieldSink", { { "NameOfClass", { "DisplacementFieldItkImageFilterSinkComponent" } } } );
  blueprint->AddComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("FixedImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageFixedInterface" } } });
  blueprint->AddConnection( "FixedImageSource", "RegistrationMethod", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("MovingImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageMovingInterface" } } });
  blueprint->AddConnection( "MovingImageSource", "RegistrationMethod", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("RegistrationMethod", "ResultImageSink", { { "NameOfInterface", { "itkImageSourceInterface" } } });
  blueprint->AddConnection( "ResampleFilter", "ResultImageSink", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("RegistrationMethod", "ResultDisplacementFieldSink", { { "NameOfInterface", { "DisplacementFieldItkImageSourceInterface" } } });
  blueprint->AddConnection( "TransformDisplacementFilter", "ResultDisplacementFieldSink", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("Metric", "RegistrationMethod", { { "NameOfInterface", { "itkMetricv4Interface" } } });
  blueprint->AddConnection( "Metric", "RegistrationMethod", { {} } );

  blueprint->AddConnection( "FixedImageSource", "Transform", { {} } );
  blueprint->AddConnection( "Transform", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "FixedImageSource", "TransformResolutionAdaptor", { {} } );
  blueprint->AddConnection( "TransformResolutionAdaptor", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "Optimizer", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->AddConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->AddConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->AddConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->AddConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->AddConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->AddConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->AddConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  blueprint->WriteBlueprint( dataManager->GetOutputFile( "itkv4_SVF_ANTSCC.dot" ) );

  // Instantiate SuperElastix
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "itkv4_SVF_ANTSCC_Image.mhd" ) );

  VectorImageWriter2DType::Pointer vectorImageWriter = VectorImageWriter2DType::New();
  vectorImageWriter->SetFileName( dataManager->GetOutputFile( "itkv4_SVF_ANTSCC_Displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );

  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  vectorImageWriter->SetInput( superElastixFilter->GetOutput< VectorImage2DType >( "ResultDisplacementFieldSink" ) );

  baselineImageReader->SetFileName( dataManager->GetBaselineFile( "itkv4_SVF_ANTSCC_Image.mhd" ) );
  baselineVectorImageReader->SetFileName( dataManager->GetBaselineFile( "itkv4_SVF_ANTSCC_Displacement.mhd" ) );

  compareImageFilter->SetTestInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  subtractVectorImageFilter->SetInput2( superElastixFilter->GetOutput< VectorImage2DType >( "ResultDisplacementFieldSink" ) );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( vectorImageWriter->Update() );

  EXPECT_NO_THROW( compareImageFilter->Update() );
  EXPECT_NO_THROW( statisticsImageFilter->Update() );

  EXPECT_EQ( 0, compareImageFilter->GetNumberOfPixelsWithDifferences() );
  EXPECT_LT( statisticsImageFilter->GetSumOutput()->Get(), 1e-16 );
}

/** Experiment 2b: ITKv4 framework, stationary velocity field transform, mean squared differences metric */
TEST_F( WBIRDemoTest, itkv4_SVF_MSD )
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  blueprint->AddComponent( "RegistrationMethod", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
                                                   { "NumberOfLevels", { "3" } },
                                                   { "ShrinkFactorsPerLevel", { "4", "2", "1" } },
                                                   { "SmoothingSigmasPerLevel", { "4", "2", "1" } } } );
  blueprint->AddComponent( "Metric", { { "NameOfClass", { "ItkMeanSquaresImageToImageMetricv4Component" } } } );
  blueprint->AddComponent( "Optimizer", { { "NameOfClass", { "ItkGradientDescentOptimizerv4Component" } },
                                          { "NumberOfIterations", { "100" } },
                                          { "LearningRate", { "0.001" } } } );
  blueprint->AddComponent( "Transform", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } } } );
  blueprint->AddComponent( "TransformResolutionAdaptor", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
                                                           { "ShrinkFactorsPerLevel", { "4", "2", "1" } } } );

  blueprint->AddComponent( "ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } } } );
  blueprint->AddComponent( "TransformDisplacementFilter", { { "NameOfClass", { "ItkTransformDisplacementFilterComponent" } } } );

  blueprint->AddComponent( "FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } } } );
  blueprint->AddComponent( "MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } } } );
  blueprint->AddComponent( "ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } } } );
  blueprint->AddComponent( "ResultDisplacementFieldSink", { { "NameOfClass", { "DisplacementFieldItkImageFilterSinkComponent" } } } );
  blueprint->AddComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("FixedImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageFixedInterface" } } });
  blueprint->AddConnection( "FixedImageSource", "RegistrationMethod", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("MovingImageSource", "RegistrationMethod", { { "NameOfInterface", { "itkImageMovingInterface" } } });
  blueprint->AddConnection( "MovingImageSource", "RegistrationMethod", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("RegistrationMethod", "ResultImageSink", { { "NameOfInterface", { "itkImageSourceInterface" } } });
  blueprint->AddConnection( "ResampleFilter", "ResultImageSink", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("RegistrationMethod", "ResultDisplacementFieldSink", { { "NameOfInterface", { "DisplacementFieldItkImageSourceInterface" } } });
  blueprint->AddConnection( "TransformDisplacementFilter", "ResultDisplacementFieldSink", { {} } );

  //optionally, tie properties to connection to avoid ambiguities
  //blueprint->AddConnection("Metric", "RegistrationMethod", { { "NameOfInterface", { "itkMetricv4Interface" } } });
  blueprint->AddConnection( "Metric", "RegistrationMethod", { {} } );

  blueprint->AddConnection( "FixedImageSource", "Transform", { {} } );
  blueprint->AddConnection( "Transform", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "FixedImageSource", "TransformResolutionAdaptor", { {} } );
  blueprint->AddConnection( "TransformResolutionAdaptor", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "Optimizer", "RegistrationMethod", { {} } );
  blueprint->AddConnection( "RegistrationMethod", "TransformDisplacementFilter", { {} } );
  blueprint->AddConnection( "FixedImageSource", "TransformDisplacementFilter", { {} } );
  blueprint->AddConnection( "RegistrationMethod", "ResampleFilter", { {} } );
  blueprint->AddConnection( "FixedImageSource", "ResampleFilter", { {} } );
  blueprint->AddConnection( "MovingImageSource", "ResampleFilter", { {} } );

  blueprint->AddConnection( "RegistrationMethod", "Controller", { {} } );          //RunRegistrationInterface
  blueprint->AddConnection( "ResampleFilter", "Controller", { {} } );              //ReconnectTransformInterface
  blueprint->AddConnection( "TransformDisplacementFilter", "Controller", { {} } ); //ReconnectTransformInterface

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  blueprint->WriteBlueprint( dataManager->GetOutputFile( "itkv4_SVF_MSD.dot" ) );

  // Instantiate SuperElastix
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "itkv4_SVF_MSD_Image.mhd" ) );

  VectorImageWriter2DType::Pointer vectorImageWriter = VectorImageWriter2DType::New();
  vectorImageWriter->SetFileName( dataManager->GetOutputFile( "itkv4_SVF_MSD_Displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );

  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  vectorImageWriter->SetInput( superElastixFilter->GetOutput< VectorImage2DType >( "ResultDisplacementFieldSink" ) );

  baselineImageReader->SetFileName( dataManager->GetBaselineFile( "itkv4_SVF_MSD_Image.mhd" ) );
  baselineVectorImageReader->SetFileName( dataManager->GetBaselineFile( "itkv4_SVF_MSD_Displacement.mhd" ) );

  compareImageFilter->SetTestInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  subtractVectorImageFilter->SetInput2( superElastixFilter->GetOutput< VectorImage2DType >( "ResultDisplacementFieldSink" ) );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  EXPECT_NO_THROW( vectorImageWriter->Update() );

  EXPECT_NO_THROW( compareImageFilter->Update() );
  EXPECT_NO_THROW( statisticsImageFilter->Update() );

  EXPECT_EQ( 0, compareImageFilter->GetNumberOfPixelsWithDifferences() );
  EXPECT_LT( statisticsImageFilter->GetSumOutput()->Get(), 1e-16 );
}

/** Experiment 1a: elastix framework, B-spline transform, normalized correlation metric */
TEST_F( WBIRDemoTest, elastix_BS_NCC )
{
  /** make blueprint configuration */
  blueprint = Blueprint::New();

  blueprint->AddComponent( "RegistrationMethod", { { "NameOfClass", { "MonolithicElastixComponent" } },
                                                   { "Transform", { "BSplineTransform" } }, { "Metric", { "AdvancedNormalizedCorrelation" } } } );

  blueprint->AddComponent( "TransformDisplacementField", { { "NameOfClass", { "MonolithicTransformixComponent" } } } );

  blueprint->AddComponent( "FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } } } );

  blueprint->AddComponent( "MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } } } );

  blueprint->AddComponent( "ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } } } );

  blueprint->AddComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  //optionally, tie properties to connection to avoid ambiguities
  blueprint->AddConnection( "FixedImageSource", "RegistrationMethod", { {} } ); // {{"NameOfInterface", { "itkImageFixedInterface" }}};

  //optionally, tie properties to connection to avoid ambiguities
  blueprint->AddConnection( "MovingImageSource", "RegistrationMethod", { {} } ); // {{"NameOfInterface", { "itkImageMovingInterface" }}};

  blueprint->AddConnection( "RegistrationMethod", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "elastixTransformParameterObjectInterface" } } } ;

  blueprint->AddConnection( "FixedImageSource", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } ;

  blueprint->AddConnection( "MovingImageSource", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "itkImageMovingInterface" } } };

  blueprint->AddConnection( "TransformDisplacementField", "ResultImageSink", { {} } ); // { { "NameOfInterface", { "itkImageInterface" } } } ;

  blueprint->AddConnection( "RegistrationMethod", "Controller", { {} } ); // { { "NameOfInterface", { "RunRegistrationInterface" } } } ;

  blueprint->AddConnection( "TransformDisplacementField", "Controller", { {} } ); // { { "NameOfInterface", { "ReconnectTransformInterface" } } } ;

  blueprint->AddConnection( "ResultImageSink", "Controller", { {} } ); // { { "NameOfInterface", { "AfterRegistrationInterface" } } } ;

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  blueprint->WriteBlueprint( dataManager->GetOutputFile( "elastix_BS_NCC.dot" ) );

  // Instantiate SuperElastix
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "elastix_BS_NCC_Image.mhd" ) );

  VectorImageWriter2DType::Pointer vectorImageWriter = VectorImageWriter2DType::New();
  vectorImageWriter->SetFileName( dataManager->GetOutputFile( "elastix_BS_NCC_Displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );

  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  //elastix component does not have an deformation output, but writes deformationfield to disk.
  //vectorImageWriter->SetInput(superElastixFilter->GetOutput<VectorImage2DType>("ResultDisplacementFieldSink"));

  baselineImageReader->SetFileName( dataManager->GetBaselineFile( "elastix_BS_NCC_Image.mhd" ) );
  baselineVectorImageReader->SetFileName( dataManager->GetBaselineFile( "elastix_BS_NCC_Displacement.mhd" ) );

  compareImageFilter->SetTestInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  //elastix component does not have an deformation output, but writes deformationfield to disk.
  //subtractVectorImageFilter->SetInput2(superElastixFilter->GetOutput<VectorImage2DType>("ResultDisplacementFieldSink"));
  VectorImageReader2DType::Pointer elastixDeformationFieldReader = VectorImageReader2DType::New();
  elastixDeformationFieldReader->SetFileName( "deformationField.nii" );
  subtractVectorImageFilter->SetInput2( elastixDeformationFieldReader->GetOutput() );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  //EXPECT_NO_THROW(vectorImageWriter->Update());

  EXPECT_NO_THROW( compareImageFilter->Update() );
  EXPECT_NO_THROW( statisticsImageFilter->Update() );

  EXPECT_EQ( 0, compareImageFilter->GetNumberOfPixelsWithDifferences() );
  EXPECT_LT( statisticsImageFilter->GetSumOutput()->Get(), 1e-16 );
}

/** Experiment 1b: elastix framework, B-spline transform, mean squared differences metric */
TEST_F( WBIRDemoTest, elastix_BS_MSD )
{
  /** make blueprint configuration */
  blueprint = Blueprint::New();

  blueprint->AddComponent( "RegistrationMethod", { { "NameOfClass", { "MonolithicElastixComponent" } },
                                                   { "Transform", { "BSplineTransform" } }, { "Metric", { "AdvancedMeanSquares" } } } );

  blueprint->AddComponent( "TransformDisplacementField", { { "NameOfClass", { "MonolithicTransformixComponent" } } } );

  blueprint->AddComponent( "FixedImageSource", { { "NameOfClass", { "ItkImageSourceFixedComponent" } } } );

  blueprint->AddComponent( "MovingImageSource", { { "NameOfClass", { "ItkImageSourceMovingComponent" } } } );

  blueprint->AddComponent( "ResultImageSink", { { "NameOfClass", { "ItkImageSinkComponent" } } } );

  blueprint->AddComponent( "Controller", { { "NameOfClass", { "RegistrationControllerComponent" } } } );

  //optionally, tie properties to connection to avoid ambiguities
  blueprint->AddConnection( "FixedImageSource", "RegistrationMethod", { {} } ); // {{"NameOfInterface", { "itkImageFixedInterface" }}};

  //optionally, tie properties to connection to avoid ambiguities
  blueprint->AddConnection( "MovingImageSource", "RegistrationMethod", { {} } ); // {{"NameOfInterface", { "itkImageMovingInterface" }}};

  blueprint->AddConnection( "RegistrationMethod", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "elastixTransformParameterObjectInterface" } } } ;

  blueprint->AddConnection( "FixedImageSource", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "itkImageDomainFixedInterface" } } } ;

  blueprint->AddConnection( "MovingImageSource", "TransformDisplacementField", { {} } ); // { { "NameOfInterface", { "itkImageMovingInterface" } } };

  blueprint->AddConnection( "TransformDisplacementField", "ResultImageSink", { {} } ); // { { "NameOfInterface", { "itkImageInterface" } } } ;

  blueprint->AddConnection( "RegistrationMethod", "Controller", { {} } ); // { { "NameOfInterface", { "RunRegistrationInterface" } } } ;

  blueprint->AddConnection( "TransformDisplacementField", "Controller", { {} } ); // { { "NameOfInterface", { "ReconnectTransformInterface" } } } ;

  blueprint->AddConnection( "ResultImageSink", "Controller", { {} } ); // { { "NameOfInterface", { "AfterRegistrationInterface" } } } ;

  // Data manager provides the paths to the input and output data for unit tests
  DataManagerType::Pointer dataManager = DataManagerType::New();

  blueprint->WriteBlueprint( dataManager->GetOutputFile( "elastix_BS_MSD.dot" ) );

  // Instantiate SuperElastix
  EXPECT_NO_THROW( superElastixFilter = SuperElastixFilterType::New() );

  // Set up the readers and writers
  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName( dataManager->GetInputFile( "coneA2d64.mhd" ) );

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName( dataManager->GetInputFile( "coneB2d64.mhd" ) );

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName( dataManager->GetOutputFile( "elastix_BS_MSD_Image.mhd" ) );

  VectorImageWriter2DType::Pointer vectorImageWriter = VectorImageWriter2DType::New();
  vectorImageWriter->SetFileName( dataManager->GetOutputFile( "elastix_BS_MSD_Displacement.mhd" ) );

  // Connect SuperElastix in an itk pipeline
  superElastixFilter->SetInput( "FixedImageSource", fixedImageReader->GetOutput() );
  superElastixFilter->SetInput( "MovingImageSource", movingImageReader->GetOutput() );

  resultImageWriter->SetInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  //elastix component does not have an deformation output, but writes deformationfield to disk.
  //vectorImageWriter->SetInput(superElastixFilter->GetOutput<VectorImage2DType>("ResultDisplacementFieldSink"));

  baselineImageReader->SetFileName( dataManager->GetBaselineFile( "elastix_BS_MSD_Image.mhd" ) );
  baselineVectorImageReader->SetFileName( dataManager->GetBaselineFile( "elastix_BS_MSD_Displacement.mhd" ) );

  compareImageFilter->SetTestInput( superElastixFilter->GetOutput< Image2DType >( "ResultImageSink" ) );
  //elastix component does not have an deformation output, but writes deformationfield to disk.
  //subtractVectorImageFilter->SetInput2(superElastixFilter->GetOutput<VectorImage2DType>("ResultDisplacementFieldSink"));
  VectorImageReader2DType::Pointer elastixDeformationFieldReader = VectorImageReader2DType::New();
  elastixDeformationFieldReader->SetFileName( "deformationField.nii" );
  subtractVectorImageFilter->SetInput2( elastixDeformationFieldReader->GetOutput() );

  EXPECT_NO_THROW( superElastixFilter->SetBlueprint( blueprint ) );

  //Optional Update call
  //superElastixFilter->Update();

  // Update call on the writers triggers SuperElastix to configure and execute
  EXPECT_NO_THROW( resultImageWriter->Update() );
  //EXPECT_NO_THROW(vectorImageWriter->Update());

  EXPECT_NO_THROW( compareImageFilter->Update() );
  EXPECT_NO_THROW( statisticsImageFilter->Update() );

  EXPECT_EQ( 0, compareImageFilter->GetNumberOfPixelsWithDifferences() );
  EXPECT_LT( statisticsImageFilter->GetSumOutput()->Get(), 1e-16 );
}
} // namespace selx
