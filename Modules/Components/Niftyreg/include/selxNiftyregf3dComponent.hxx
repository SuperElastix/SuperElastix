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

#include <nifti1_io.h>
#include "selxNiftyregf3dComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
Niftyregf3dComponent< TPixel >::Niftyregf3dComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  this->m_reg_f3d = new reg_f3d< TPixel >( 1, 1 );

  // Set spacing to positive values in mm. The default values are (-5 -5 -5) which NiftyReg interprets as spacing in voxels.
  this->m_reg_f3d->SetSpacing(0, 5);
  this->m_reg_f3d->SetSpacing(1, 5);
  this->m_reg_f3d->SetSpacing(2, 5);

  this->m_reg_f3d->SetWarpedPaddingValue(0.);
}


template< class TPixel >
Niftyregf3dComponent< TPixel >::~Niftyregf3dComponent()
{
  delete m_reg_f3d;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Accept( typename NiftyregReferenceImageInterface< TPixel >::Pointer component )
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_reference_image = component->GetReferenceNiftiImage();
  this->m_reg_f3d->SetReferenceImage( this->m_reference_image.get() );
  return 0;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Accept( typename NiftyregFloatingImageInterface< TPixel >::Pointer component )
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_floating_image = component->GetFloatingNiftiImage();
  this->m_reg_f3d->SetFloatingImage( this->m_floating_image.get() );
  return 0;
}

template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Accept( typename NiftyregAffineMatrixInterface::Pointer component )
{
  this->m_NiftyregAffineMatrixInterface = component;
  return 0;
}

template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Accept( typename NiftyregControlPointPositionImageInterface< TPixel >::Pointer component )
{
  this->m_reg_f3d->SetControlPointGridImage(component->GetControlPointPositionImage().get());
  return 0;
}


template< class TPixel >
int
Niftyregf3dComponent< TPixel >
::Accept(typename NiftyregInputMaskInterface<  unsigned char  >::Pointer component)
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_input_mask = component->GetInputMask();
  // connect the itk pipeline
  this->m_reg_f3d->SetReferenceMask( this->m_input_mask.get() );
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
Niftyregf3dComponent< TPixel >
::Update()
{
  this->m_Logger.Log(LogLevel::TRC, "Update: run registration");
  //this->m_reg_f3d->UseSSD( 0, true );
  //this->m_reg_f3d->UseCubicSplineInterpolation();
  if (this->m_NiftyregAffineMatrixInterface)
  {
    this->m_reg_f3d->SetAffineTransformation(this->m_NiftyregAffineMatrixInterface->GetAffineNiftiMatrix());
  }
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

template< class TPixel >
bool
Niftyregf3dComponent< TPixel >
::ConnectionsSatisfied()
{
  this->m_Logger.Log(LogLevel::TRC, "Niftyregf3dComponent::ConnectionsSatisfied tests if NiftyregReferenceImageInterface and NiftyregFloatingImageInterface are connected.");
  // This function overrides the default behavior, in which all accepting interfaces must be set, by allowing the some interfaces not being set.
  // TODO: see if we can reduce the amount of code with helper (meta-)functions
  //   Superclass::AcceptingInterfacesTypeList
  if ( !this->InterfaceAcceptor< NiftyregReferenceImageInterface< TPixel >>::GetAccepted() )
  {
    return false;
  }
  if ( !this->InterfaceAcceptor< NiftyregFloatingImageInterface< TPixel >>::GetAccepted() )
  {
    return false;
  }

  // Allow other accepting interfaces to be unconnected
  return true;
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
  else if( criterion.first == "NumberOfIterations" || criterion.first == "MaximumNumberOfIterations" || criterion.first == "MaximalIterationNumber" ) //Supports this?
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
  else if( criterion.first == "GridSpacingInPhysicalUnits" ) //Supports this?
  {
    for( unsigned int d = 0; d < criterion.second.size(); ++d )
    {
      this->m_reg_f3d->SetSpacing( d, -std::stof( criterion.second[ d ] ) );
    }
    meetsCriteria = true;
  }
  else if( criterion.first == "SmoothingSigma" )
  {
    meetsCriteria = true;
    if( criterion.second.size() > 1 )
    {
      this->Error("NiftyReg does not support multiple smoothing sigmas. Only the first image is smoothed.");
      meetsCriteria = false;
    }
    this->m_reg_f3d->SetReferenceSmoothingSigma( std::stof( criterion.second[ 0 ] ) );
    this->m_reg_f3d->SetFloatingSmoothingSigma( std::stof( criterion.second[ 0 ] ) );
  }
  return meetsCriteria;
}
} //end namespace selx
