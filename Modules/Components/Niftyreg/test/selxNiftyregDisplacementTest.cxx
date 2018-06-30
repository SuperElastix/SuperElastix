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

TEST_F( NiftyregDisplacementTest, displacement_conversion )
{
  /** make example blueprint configuration */

  // Generate an nifti image on the fly
  int size = 64;
  int dim[8] = {3, size, size, size, 1, 1, 1, 1};
  nifti_image *floating_image = nifti_make_new_nim(dim, NIFTI_TYPE_FLOAT32, true);
  float *imgPtr = static_cast<float>(floating_image->data);
  for(int i=0; i<floating_image->nvox; ++i){
    imgPtr[i] = i;
  }
  // Create a deformation field that embeds an affine transformation
  mat44 affine;
  affine.m[0][0]=0.8.f;affine.m[0][1]=0.1f;affine.m[0][2]=0.1f;affine.m[0][3]=-5.f;
  affine.m[0][0]=0.1f;affine.m[0][1]=1.1f;affine.m[0][2]=0.1f;affine.m[0][3]=1.f;
  affine.m[0][0]=0.1f;affine.m[0][1]=0.1f;affine.m[0][2]=1.f;affine.m[0][3]=5.f;
  affine.m[0][0]=0.f;affine.m[0][1]=0.f;affine.m[0][2]=0.f;affine.m[3][3]=1.f;
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
  warped_image->data = (void *)calloc(warped_image->nvox,warped_image->nbyper);
  reg_resampleImage(floating_image,
                    warped_image,
                    transFieldImage,
                    NULL,
                    0, // nearest neighbour interpolation
                    0); // 0 padding

  // Convert the deformation field into a displacement field
  reg_getDisplacementFromDeformation(transFieldImage);

  // NEED TO ADD THE SELX PART HERE

  // NEED TO ADD THE COMPARISON HERE

  nifti_image_free(floating_image);
  nifti_image_free(warped_image);
  nifti_image_free(transFieldImage);
}
}
