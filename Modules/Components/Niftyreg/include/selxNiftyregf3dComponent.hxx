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

#include "selxNiftyregf3dComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
Niftyregf3dComponent< TPixel >::Niftyregf3dComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  m_reg_f3d = new reg_f3d< TPixel >( 1, 1 );
}


template< class TPixel >
Niftyregf3dComponent< TPixel >::~Niftyregf3dComponent()
{
  delete m_reg_f3d;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Set( typename NiftyregReferenceImageInterface< TPixel >::Pointer component )
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_reference_image = component->GetReferenceNiftiImage();
  // connect the itk pipeline
  this->m_reg_f3d->SetReferenceImage( this->m_reference_image.get() );
  return 0;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Set( typename NiftyregFloatingImageInterface< TPixel >::Pointer component )
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_floating_image = component->GetFloatingNiftiImage();
  // connect the itk pipeline
  this->m_reg_f3d->SetFloatingImage( this->m_floating_image.get() );
  return 0;
}


template< class TPixel >
std::shared_ptr< nifti_image >
Niftyregf3dComponent< TPixel >
::GetWarpedNiftiImage()
{
  return ( *( this->m_warped_images.get() ) )[ 0 ];
}

template< class TPixel >
std::shared_ptr< nifti_image >
Niftyregf3dComponent< TPixel >
::GetControlPointPositionImage()
{
  return this->m_cpp_image;
}

template< class TPixel >
void
Niftyregf3dComponent<  TPixel >
::RunRegistration()
{
  //this->m_reg_f3d->UseSSD( 0, true );
  //this->m_reg_f3d->UseCubicSplineInterpolation();

  this->m_reg_f3d->Run();
  nifti_image ** outputWarpedImage = m_reg_f3d->GetWarpedImage();
  memset( outputWarpedImage[ 0 ]->descrip, 0, 80 );
  strcpy( outputWarpedImage[ 0 ]->descrip, "Warped image using NiftyReg (reg_f3d) via SuperElastix" );

  //encapsulate malloc-ed pointer in a smartpointer for proper memory ownership
  this->m_warped_images
    = std::unique_ptr< std::array< std::shared_ptr< nifti_image >,
    2 >>( new std::array< std::shared_ptr< nifti_image >, 2 > );
  (*(this->m_warped_images.get()))[0] = std::shared_ptr< nifti_image >(outputWarpedImage[0], nifti_image_free );
  (*(this->m_warped_images.get()))[1] = std::shared_ptr< nifti_image >(outputWarpedImage[1], nifti_image_free );

  // m_reg_f3d->GetWarpedImage() malloc-ed the container which we must free ourselves.
  free( outputWarpedImage );
  outputWarpedImage = NULL;

  this->m_cpp_image = std::shared_ptr< nifti_image >(m_reg_f3d->GetControlPointPositionImage(), nifti_image_free );

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
Niftyregf3dComponent<  TPixel >
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

  else if( criterion.first == "Metric" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      if( criterion.second[ 0 ] == "SumOfSquaredDifferences" || criterion.second[ 0 ] == "SSD" )
      {
        this->m_reg_f3d->UseSSD( 0, false );// try catch? TODO normalize argument?
      }
      else if( criterion.second[ 0 ] == "MIND" )
      {
        this->m_reg_f3d->UseMIND( 0, 0 );// try catch? TODO timepoint, offset arguments?
      }
      else if( criterion.second[ 0 ] == "MINDSCC" )
      {
        this->m_reg_f3d->UseMINDSSC( 0, 0 );// try catch? TODO timepoint, offset arguments?
      }
      else if( criterion.second[ 0 ] == "KLDivergence" )
      {
        this->m_reg_f3d->UseKLDivergence( 0 );// try catch?
      }
      else if( criterion.second[ 0 ] == "DTI" )
      {
        this->m_reg_f3d->UseDTI( 0 );// try catch?
      }
      else if( criterion.second[ 0 ] == "LNCC" )
      {
        this->m_reg_f3d->UseLNCC( 0, 5.0 );// try catch? TODO stdDevKernel;
      }
      else
      {
        this->Warning( "Criterion {0} has invalid value.", criterion.first );
        return false;
      }
    }
    else
    {
      this->Error( "Criteria {0} does not accept multiple values." );
      return false;
    }
  }
  else if( criterion.first == "NumberOfIterations" || criterion.first == "MaximalIterationNumber" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      // try catch?
      this->m_reg_f3d->SetMaximalIterationNumber( std::stoi( criterion.second[ 0 ] ) );
    }
    else
    {
      this->Warning( "{0} does not accept multiple values.", criterion.first );
      return false;
    }
  }

  else if( criterion.first == "Optimizer" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      if( criterion.second[ 0 ] == "ConjugateGradient" )
      {
        this->m_reg_f3d->UseConjugateGradient();// try catch?
      }
      else if( criterion.second[ 0 ] == "Gradient" )
      {
        this->m_reg_f3d->DoNotUseConjugateGradient();// try catch?
      }
      else
      {
        this->Warning( "Criterion {0} has invalid value.", criterion.first);
        return false;
      }
    }
    else
    {
      // TODO log error?
      this->Warning( "{0} does not accept multiple values.", criterion.first );
      return false;
    }
  }

  else if( criterion.first == "NumberOfResolutions" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      // try catch?
      this->m_reg_f3d->SetLevelNumber( std::stoi( criterion.second[ 0 ] ) );
    }
    else
    {
      this->Warning( "{0} does not accept multiple values.", criterion.first );
      return false;
    }
  }
  else if( criterion.first == "GridSpacingInVoxels" ) //Supports this?
  {
    for( unsigned int d = 0; d < criterion.second.size(); ++d )
    {
      this->m_reg_f3d->SetSpacing( d, std::stof( criterion.second[ d ] ) );
    }
    meetsCriteria = true;
  }
  return meetsCriteria;
}
} //end namespace selx
