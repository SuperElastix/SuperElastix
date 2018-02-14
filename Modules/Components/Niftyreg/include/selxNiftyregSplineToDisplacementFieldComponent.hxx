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

#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
NiftyregSplineToDisplacementFieldComponent< TPixel >::NiftyregSplineToDisplacementFieldComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
}


template< class TPixel >
NiftyregSplineToDisplacementFieldComponent< TPixel >::~NiftyregSplineToDisplacementFieldComponent()
{
}


template< class TPixel >
int
NiftyregSplineToDisplacementFieldComponent< TPixel >
::Accept( typename NiftyregReferenceImageInterface< TPixel >::Pointer component )
{
  // store the shared_ptr, otherwise the data might get freed
  this->m_reference_image = component->GetReferenceNiftiImage();
  return 0;
}

template< class TPixel >
int
NiftyregSplineToDisplacementFieldComponent< TPixel >
::Accept(typename NiftyregControlPointPositionImageInterface< TPixel >::Pointer component)
{
  this->m_NiftyregControlPointPositionImageInterface = component;
  return 0;
}


template< class TPixel >
void
NiftyregSplineToDisplacementFieldComponent<  TPixel >
::Update()
{
  
  nifti_image * inputTransformationImage = this->m_NiftyregControlPointPositionImageInterface->GetControlPointPositionImage().get();
  
  // Create a dense field
  if (
    inputTransformationImage->intent_p1 == LIN_SPLINE_GRID ||
    inputTransformationImage->intent_p1 == CUB_SPLINE_GRID ||
    inputTransformationImage->intent_p1 == SPLINE_VEL_GRID)
  {
  }
  // Create a field image from the reference image
  nifti_image * outputTransformationImage = nifti_copy_nim_info(this->m_reference_image.get());
  outputTransformationImage->ndim = outputTransformationImage->dim[0] = 5;
  outputTransformationImage->nt = outputTransformationImage->dim[4] = 1;
  outputTransformationImage->nu = outputTransformationImage->dim[5] = outputTransformationImage->nz > 1 ? 3 : 2;
  outputTransformationImage->nvox = (size_t)outputTransformationImage->nx *
    outputTransformationImage->ny * outputTransformationImage->nz *
    outputTransformationImage->nt * outputTransformationImage->nu;
  outputTransformationImage->nbyper = sizeof(float);
  outputTransformationImage->datatype = NIFTI_TYPE_FLOAT32;
  outputTransformationImage->intent_code = NIFTI_INTENT_VECTOR;
  memset(outputTransformationImage->intent_name, 0, 16);
  strcpy(outputTransformationImage->intent_name, "NREG_TRANS");
  outputTransformationImage->scl_slope = 1.f;
  outputTransformationImage->scl_inter = 0.f;

  // Allocate the output field data array
  outputTransformationImage->data = (void *)malloc
    (outputTransformationImage->nvox*outputTransformationImage->nbyper);

  printf("[NiftyReg] The specified transformation is a spline parametrisation:\n[NiftyReg] %s\n",
    inputTransformationImage->fname);
  // The output field is filled with an identity deformation field
  memset(outputTransformationImage->data,
    0,
    outputTransformationImage->nvox*outputTransformationImage->nbyper);
  
  reg_getDeformationFromDisplacement(outputTransformationImage);
  // The spline transformation is composed with the identity field
  //TODO: replace reg_spline_getDeformationField by reg_cubic_spline_getDeformationField2D<TPixel>
  reg_spline_getDeformationField(inputTransformationImage,
    outputTransformationImage,
    NULL, // no mask
    true, // composition is used,
    true // b-spline are used
    );

  reg_getDisplacementFromDeformation(outputTransformationImage);
  this->m_displacement_image = std::shared_ptr< nifti_image >(outputTransformationImage, [](nifti_image* ptr){nifti_image_free(ptr); });
  
  return;
}

template< class TPixel >
std::shared_ptr< nifti_image >
NiftyregSplineToDisplacementFieldComponent< TPixel >
::GetDisplacementFieldNiftiImage()
{
  return this->m_displacement_image;
}

/* Niftyreg member functions:
void SetControlPointGridImage(nifti_image *);
void SetBendingEnergyWeight(T);
void SetLinearEnergyWeight(T);
void SetJacobianLogWeight(T);
void ApproximateJacobianLog();
void DoNotApproximateJacobianLog();
void SetSpacing(unsigned int, T);
void NoGridRefinement()
void SetMaximalIterationNumber(unsigned int);
void NoOptimisationAlongX()
void NoOptimisationAlongY()
void NoOptimisationAlongZ()
void SetPerturbationNumber(size_t v)
void UseConjugateGradient();
void DoNotUseConjugateGradient();
void UseApproximatedGradient();
void DoNotUseApproximatedGradient();
// Measure of similarity related functions
//    void ApproximateParzenWindow();
//    void DoNotApproximateParzenWindow();
virtual void UseNMISetReferenceBinNumber(int, int);
virtual void UseNMISetFloatingBinNumber(int, int);
virtual void UseSSD(int timepoint, bool normalize);
virtual void UseMIND(int timepoint, int offset);
virtual void UseMINDSSC(int timepoint, int offset);
virtual void UseKLDivergence(int timepoint);
virtual void UseDTI(bool *timepoint);
virtual void UseLNCC(int timepoint, float stdDevKernel);
virtual void SetLNCCKernelType(int type);
void SetLocalWeightSim(nifti_image *);

void SetNMIWeight(int, double);
void SetSSDWeight(int, double);
void SetKLDWeight(int, double);
void SetLNCCWeight(int, double);

void SetReferenceImage(nifti_image *);
void SetFloatingImage(nifti_image *);
void SetReferenceMask(nifti_image *);
void SetAffineTransformation(mat44 *);
void SetReferenceSmoothingSigma(T);
void SetFloatingSmoothingSigma(T);
void SetGradientSmoothingSigma(T);
void SetReferenceThresholdUp(unsigned int, T);
void SetReferenceThresholdLow(unsigned int, T);
void SetFloatingThresholdUp(unsigned int, T);
void SetFloatingThresholdLow(unsigned int, T);
void UseRobustRange();
void DoNotUseRobustRange();
void SetWarpedPaddingValue(T);
void SetLevelNumber(unsigned int);
void SetLevelToPerform(unsigned int);
void PrintOutInformation();
void DoNotPrintOutInformation();
void DoNotUsePyramidalApproach();
void UseNeareatNeighborInterpolation();
void UseLinearInterpolation();
void UseCubicSplineInterpolation();
void SetLandmarkRegularisationParam(size_t, float *, float*, float);
*/
template< class TPixel >
bool
NiftyregSplineToDisplacementFieldComponent<  TPixel >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  } // else: CriterionStatus::Unknown

  return meetsCriteria;
}
} //end namespace selx
