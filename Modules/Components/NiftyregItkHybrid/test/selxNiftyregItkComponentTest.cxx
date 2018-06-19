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

#include "selxNiftyregItkMultiStageComponent.h"
#include "selxItkToNiftiImageHybridSourceComponent.h"

#include "selxNiftiToItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"

#include "selxItkImageSinkComponent.h"
#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component.h"
#include "selxItkMeanSquaresImageToImageMetricv4Component.h"
#include "selxItkGradientDescentOptimizerv4Component.h"
#include "selxItkAffineTransformComponent.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformComponent.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.h"
#include "selxItkResampleFilterComponent.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

namespace selx
{
class NiftyregItkHybridComponentTest : public ::testing::Test
{
public:

  typedef Blueprint::Pointer BlueprintPointer;
  typedef Blueprint::ParameterMapType ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef DataManager DataManagerType;

  /** register all example components */
  typedef TypeList< 
    NiftyregItkMultiStageComponent<double, 2>,
	  Niftyregf3dComponent< float >,
    ItkToNiftiImageHybridSourceComponent< 2, float >,
    NiftiToItkImageSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkToNiftiImageHybridSourceComponent< 3, float >,
    NiftiToItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 3, float >,
    NiftyregSplineToDisplacementFieldComponent< float>,
    DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
    NiftyregAladinComponent< float >,
    NiftyregAladinComponent< double >,
    ItkImageSinkComponent< 3, float >,
    ItkImageSinkComponent< 2, float >,
    ItkImageRegistrationMethodv4Component< 3, float, double >,
    ItkImageRegistrationMethodv4Component< 2, float, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 3, double >,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
    ItkMeanSquaresImageToImageMetricv4Component< 3, float, double >,
    ItkMeanSquaresImageToImageMetricv4Component< 2, float, double  >,
    ItkGradientDescentOptimizerv4Component< double >,
    ItkAffineTransformComponent< double, 3 >,
    ItkAffineTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 3 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 3, double >,
    ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
    ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
    ItkResampleFilterComponent< 2, float, double >,
    ItkResampleFilterComponent< 3, float, double >
    > RegisterComponents;

  typedef SuperElastixFilterCustomComponents< RegisterComponents > SuperElastixFilterType;

  virtual void SetUp()
  {
    // Instantiate SuperElastixFilter before each test and
    // register the components we want to have available in SuperElastix
    superElastixFilter = SuperElastixFilterCustomComponents< RegisterComponents >::New();
    dataManager = DataManagerType::New();
    logger = Logger::New();
    logger->AddStream("cout", std::cout);
    //auto logfile = std::ofstream("NiftyregItkHybridComponentTest.txt");
    //logger->AddStream("logfile", logfile, true);
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


TEST_F(NiftyregItkHybridComponentTest, AladinAndItk)
{
  /** make example blueprint configuration */
  BlueprintPointer blueprint = Blueprint::New();

  blueprint->SetComponent("MultiStageTransformController", { { "NameOfClass", { "NiftyregItkMultiStageComponent" } }, {"ExecutionOrder" , { "RegistrationMethod1", "RegistrationMethod2" } } });

  blueprint->SetComponent("RegistrationMethod1", { { "NameOfClass", { "NiftyregAladinComponent" } } });
  blueprint->SetComponent("FixedImage", { { "NameOfClass", { "ItkToNiftiImageHybridSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("MovingImage", { { "NameOfClass", { "ItkToNiftiImageHybridSourceComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });
  blueprint->SetComponent("ResultImage", { { "NameOfClass", { "ItkImageSinkComponent" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } });

  blueprint->SetConnection("FixedImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregReferenceImageInterface" } } });
  blueprint->SetConnection("MovingImage", "RegistrationMethod1", { { "NameOfInterface", { "NiftyregFloatingImageInterface" } } });

  blueprint->SetComponent("ResampleFilter", { { "NameOfClass", { "ItkResampleFilterComponent" } } });
  blueprint->SetConnection("FixedImage", "ResampleFilter", { {} });
  blueprint->SetConnection("MovingImage", "ResampleFilter", { {} });
  blueprint->SetConnection("MultiStageTransformController", "ResampleFilter", { {} });

  blueprint->SetConnection("ResampleFilter", "ResultImage", {});

  blueprint->SetConnection("RegistrationMethod1", "MultiStageTransformController", { { "NameOfInterface", { "NiftyregAffineMatrixInterface"} } }, "NiftyregAffineMatrixInterface"); 
  blueprint->SetConnection("RegistrationMethod1", "MultiStageTransformController", { { "NameOfInterface", { "UpdateInterface"} } }, "UpdateInterface");

  blueprint->SetComponent("RegistrationMethod2", { { "NameOfClass", { "ItkImageRegistrationMethodv4Component" } },
  { "Dimensionality", { "2" } },
  { "InternalComputationValueType", { "double" } },
  { "PixelType", { "float" } },
  { "NumberOfLevels", { "2" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } },
  { "SmoothingSigmasPerLevel", { "2", "1" } } });
  blueprint->SetConnection("FixedImage", "RegistrationMethod2", {});
  blueprint->SetConnection("MovingImage", "RegistrationMethod2", {});

  blueprint->SetComponent("Metric2", { { "NameOfClass", { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" } } });
  blueprint->SetConnection("Metric2", "RegistrationMethod2", {});
  blueprint->SetComponent("Transform2", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformComponent" } } });
  blueprint->SetConnection("Transform2", "RegistrationMethod2", {});
  blueprint->SetConnection("FixedImage", "Transform2", {});
  blueprint->SetComponent("TransformResolutionAdaptor2", { { "NameOfClass", { "ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent" } },
  { "ShrinkFactorsPerLevel", { "2", "1" } } });
  blueprint->SetConnection("FixedImage", "TransformResolutionAdaptor2", {});
  blueprint->SetConnection("TransformResolutionAdaptor2", "RegistrationMethod2", {});

  blueprint->SetComponent("Optimizer2", { { "NameOfClass", { "ItkGradientDescentOptimizerv4Component" } } });
  blueprint->SetConnection("Optimizer2", "RegistrationMethod2", {});
   
  blueprint->SetConnection("RegistrationMethod2", "MultiStageTransformController", { { "NameOfInterface", { "MultiStageTransformInterface"} } }, "MultiStageTransformInterface");
  blueprint->SetConnection("RegistrationMethod2", "MultiStageTransformController", { { "NameOfInterface", { "UpdateInterface"} } }, "UpdateInterface");


  blueprint->Write(dataManager->GetOutputFile("NiftyregItkHybrid_AladinAndItk.dot"));

  // Set up the readers and writers
  typedef itk::Image< float, 2 >              Image2DType;
  typedef itk::ImageFileReader< Image2DType > ImageReader2DType;
  typedef itk::ImageFileWriter< Image2DType > ImageWriter2DType;

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("coneA2d64_shift.mhd"));

  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("NiftyregItkHybrid_AladinAndItk_Image.mhd"));

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


}
