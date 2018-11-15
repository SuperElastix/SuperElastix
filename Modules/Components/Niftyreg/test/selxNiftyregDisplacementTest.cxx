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
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"

#include "itkDisplacementFieldTransform.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkTestingComparisonImageFilter.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

#include "_reg_tools.h"

namespace selx
{
class NiftyregDisplacementTest : public ::testing::Test
{
public:

  typedef Blueprint::Pointer BlueprintPointer;
  typedef Blueprint::ParameterMapType ParameterMapType;
  typedef Blueprint::ParameterValueType ParameterValueType;
  typedef DataManager DataManagerType;

  /** register all example components */
  typedef TypeList<
    ItkToNiftiImageSourceComponent< 2, float >,
    NiftiToItkImageSinkComponent< 2, float >,
    ItkImageSourceComponent< 2, float >,
    ItkToNiftiImageSourceComponent< 3, float >,
    NiftiToItkImageSinkComponent< 3, float >,
    ItkImageSourceComponent< 3, float >,
    DisplacementFieldNiftiToItkImageSinkComponent< 2, float>> RegisterComponents;

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

// TODO: Get this test working again after the NiftyReg fix
TEST_F( NiftyregDisplacementTest, DISABLED_displacement_conversion )
{
  /** make example blueprint configuration */

  // Generate an nifti image on the fly
  int size = 64;
  int dim[8] = {3, size, size, size, 1, 1, 1, 1};
  nifti_image *floating_image = nifti_make_new_nim(dim, NIFTI_TYPE_FLOAT32, true);
  floating_image->qform_code = 1;
  float *imgPtr = static_cast<float*>(floating_image->data);
  for(int i=0; i<floating_image->nvox; ++i){
    imgPtr[i] = i;
  }
  // Create a deformation field that embeds an affine transformation
  mat44 affine;
  affine.m[0][0]=0.8f;affine.m[0][1]=0.1f;affine.m[0][2]=0.1f;affine.m[0][3]=-5.f;
  affine.m[1][0]=0.1f;affine.m[1][1]=1.1f;affine.m[1][2]=0.1f;affine.m[1][3]=1.f;
  affine.m[2][0]=0.1f;affine.m[2][1]=0.1f;affine.m[1][2]=1.f;affine.m[2][3]=5.f;
  affine.m[3][0]=0.f;affine.m[3][1]=0.f;affine.m[1][2]=0.f;affine.m[3][3]=1.f;
  //reg_mat44_eye(&affine);
  nifti_image *transFieldImage = nifti_copy_nim_info(floating_image);
  transFieldImage->dim[0]=transFieldImage->ndim=5;
  transFieldImage->dim[1]=transFieldImage->nx=floating_image->nx;
  transFieldImage->dim[2]=transFieldImage->ny=floating_image->ny;
  transFieldImage->dim[3]=transFieldImage->nz=floating_image->nz;
  transFieldImage->dim[4]=transFieldImage->nt=1;
  transFieldImage->pixdim[4]=transFieldImage->dt=1.0;
  transFieldImage->dim[5]=transFieldImage->nu=floating_image->nz>1?3:2;
  transFieldImage->dim[6]=transFieldImage->nv=1;
  transFieldImage->dim[7]=transFieldImage->nw=1;
  transFieldImage->nvox =(size_t)transFieldImage->nx*
        transFieldImage->ny*transFieldImage->nz*
        transFieldImage->nt*transFieldImage->nu;
  transFieldImage->scl_slope=1.f;
  transFieldImage->scl_inter=0.f;
   transFieldImage->datatype = NIFTI_TYPE_FLOAT32;
   transFieldImage->nbyper = sizeof(float);
  transFieldImage->data = (void *)calloc(transFieldImage->nvox, transFieldImage->nbyper);
  reg_affine_getDeformationField(&affine,
                                 transFieldImage);

  // Apply the deformation to the floating image
  nifti_image* warped_image = nifti_copy_nim_info(floating_image);
  warped_image->data = (void *)calloc(warped_image->nvox, warped_image->nbyper);
  reg_resampleImage(floating_image,
                    warped_image,
                    transFieldImage,
                    NULL,
                    0, // nearest neighbour interpolation
                    0); // 0 padding

  // write warped to disk 
  reg_io_WriteImageFile(warped_image,dataManager->GetOutputFile("NiftyregDisplacementTest_displacement_conversion_NiftyReg_warped.nii").c_str());

  // write deformation field to disk 
  reg_io_WriteImageFile(transFieldImage, dataManager->GetOutputFile("NiftyregDisplacementTest_displacement_conversion_NiftyReg_deformation.nii").c_str());

  // Convert the deformation field into a displacement field for itk
  reg_getDisplacementFromDeformation(transFieldImage);
  float *dispPtrX = static_cast<float *>(transFieldImage->data);
  float *dispPtrY = static_cast<float *>(&dispPtrX[warped_image->nvox]);
  float *dispPtrZ = static_cast<float *>(&dispPtrY[warped_image->nvox]);
  for (int v = 0; v<warped_image->nvox; v++) {
	  dispPtrX[v] *= -1.f;
	  dispPtrY[v] *= -1.f;
  }

  // write displacement field to disk 
  reg_io_WriteImageFile(transFieldImage, dataManager->GetOutputFile("NiftyregDisplacementTest_displacement_conversion_NiftyReg_displacement.nii").c_str());

  // NEED TO ADD THE ITK PART HERE
  
  using itkImageType = itk::Image<float, 3>;
  std::shared_ptr< nifti_image > nifti_floating_image(floating_image, nifti_image_free);
  auto itkImage = selx::NiftiToItkImage<itkImageType, float>::Convert(nifti_floating_image);
  using CoordRepType = float;

  using itkDisplacementType = itk::Image<itk::Vector<CoordRepType,3>, 3>;
  std::shared_ptr< nifti_image > nifti_transFieldImage(transFieldImage, nifti_image_free);
  auto itkDisplacementImage = selx::NiftiToItkImage<itkDisplacementType, float>::Convert(nifti_transFieldImage);
  
  // write displacement field to disk 
  auto itkDisplacementWriter = itk::ImageFileWriter<itkDisplacementType>::New();
  itkDisplacementWriter->SetFileName(dataManager->GetOutputFile("NiftyregDisplacementTest_displacement_conversion_itk_displacement.nii"));
  itkDisplacementWriter->SetInput(itkDisplacementImage);
  itkDisplacementWriter->Update();

  using DisplacementFieldTransformType = itk::DisplacementFieldTransform< CoordRepType, 3 >;
  using DisplacementFieldTransformPointer = typename DisplacementFieldTransformType::Pointer;

  using ResampleImageFilterType = itk::ResampleImageFilter< itkImageType, itkImageType, CoordRepType >;
  using ResampleImageFilterPointer = typename ResampleImageFilterType::Pointer;

  using NearestNeighborInterpolatorType = itk::NearestNeighborInterpolateImageFunction<
	  itkImageType, typename itkImageType::PixelType>;
  using NearestNeighborInterpolatorTypePointer = typename NearestNeighborInterpolatorType::Pointer;

  auto displacementFieldTransform = DisplacementFieldTransformType::New();
  auto resampleImageFilter = ResampleImageFilterType::New();
  resampleImageFilter->SetInput(itkImage);
  resampleImageFilter->SetTransform(displacementFieldTransform.GetPointer());
  auto nearestNeighborInterpolator = NearestNeighborInterpolatorType::New();
  resampleImageFilter->SetInterpolator(nearestNeighborInterpolator);

  itkDisplacementImage->Update();
  displacementFieldTransform->SetDisplacementField(itkDisplacementImage);
  resampleImageFilter->SetOutputParametersFromImage(itkDisplacementImage);
  //resampleImageFilter->Update();
  auto itkwarped = resampleImageFilter->GetOutput();

  auto itkwriter = itk::ImageFileWriter<itkImageType>::New();
  itkwriter->SetInput(itkwarped);
  itkwriter->SetFileName(dataManager->GetOutputFile("NiftyregDisplacementTest_displacement_conversion_itk_warped.nii"));
  itkwriter->Update();

  // NEED TO ADD THE COMPARISON HERE
  std::shared_ptr< nifti_image > nifti_warped_image(warped_image, nifti_image_free);
  auto itk_nifti_warped_image = selx::NiftiToItkImage<itkImageType, float>::Convert(nifti_warped_image);

  auto diff = itk::Testing::ComparisonImageFilter<itkImageType,itkImageType>::New();
  diff->SetValidInput(itk_nifti_warped_image);
  //diff->SetTestInput(itk_nifti_warped_image);
  //diff->SetValidInput(resampleImageFilter->GetOutput());
  diff->SetTestInput(resampleImageFilter->GetOutput());
  diff->SetDifferenceThreshold(0.0);
  //diff->SetToleranceRadius(radiusTolerance);
  diff->UpdateLargestPossibleRegion();
  bool differenceFailed = false;
  const double averageIntensityDifference = diff->GetTotalDifference();
  EXPECT_FLOAT_EQ(0.0, averageIntensityDifference);


  //nifti_image_free(floating_image);
  //nifti_image_free(warped_image);
  //nifti_image_free(transFieldImage);
}


}
