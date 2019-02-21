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

#include "selxNiftyregAladinComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
NiftyregAladinComponent< TPixel >::NiftyregAladinComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  m_reg_aladin = new reg_aladin< TPixel >();
  m_reg_aladin->SetAlignCentre(true);
}


template< class TPixel >
NiftyregAladinComponent< TPixel >::~NiftyregAladinComponent()
{
  delete m_reg_aladin;
}


template< class TPixel >
int
NiftyregAladinComponent< TPixel >
::Accept(typename NiftyregReferenceImageInterface< TPixel >::Pointer component)
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_reference_image = component->GetReferenceNiftiImage();
  // connect the itk pipeline
  this->m_reg_aladin->SetInputReference( this->m_reference_image.get() );
  return 0;
}


template< class TPixel >
int
NiftyregAladinComponent< TPixel >
::Accept(typename NiftyregFloatingImageInterface< TPixel >::Pointer component)
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_floating_image = component->GetFloatingNiftiImage();
  // connect the itk pipeline
  this->m_reg_aladin->SetInputFloating(this->m_floating_image.get());
  return 0;
}


template< class TPixel >
int
NiftyregAladinComponent< TPixel >
::Accept(typename NiftyregInputMaskInterface<  unsigned char  >::Pointer component)
{
  // store the shared_ptr to the data, otherwise it gets freed
  this->m_input_mask = component->GetInputMask();
  // connect the itk pipeline
  this->m_reg_aladin->SetInputMask( this->m_input_mask.get() );
  return 0;
}


template< class TPixel >
std::shared_ptr< nifti_image >
NiftyregAladinComponent< TPixel >
::GetWarpedNiftiImage()
{
  return this->m_warped_image;
}

template< class TPixel >
mat44 *
NiftyregAladinComponent< TPixel >
::GetAffineNiftiMatrix()
{
  return this->m_reg_aladin->GetTransformationMatrix();
}

template< class TPixel >
void
NiftyregAladinComponent<  TPixel >
::Update()
{
  this->m_Logger.Log(LogLevel::TRC, "Update: run registration");
  this->m_reg_aladin->Run();
  nifti_image * outputWarpedImage = m_reg_aladin->GetFinalWarpedImage();
  memset( outputWarpedImage->descrip, 0, 80 );
  strcpy( outputWarpedImage->descrip, "Warped image using NiftyReg (reg_aladin)" );
  
  
  //encapsulate malloc-ed pointer in a smartpointer for proper memory ownership
  this->m_warped_image = std::shared_ptr< nifti_image >(outputWarpedImage, nifti_image_free);
}


template< class TPixel >
bool
NiftyregAladinComponent<  TPixel >
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

  else if (criterion.first == "NumberOfIterations" || criterion.first == "maxit" || criterion.first == "MaximumNumberOfIterations") //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      // try catch?
      this->m_reg_aladin->SetMaxIterations(std::stoi(criterion.second[0]));
    }
    else
    {
      // TODO log error?
      std::cout << "NumberOfIterations accepts one number only" << std::endl;
      return false;
    }
  }
  else if (criterion.first == "NumberOfResolutions" || criterion.first == "ln" || criterion.first == "NumberOfLevels") //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
      // try catch?
      this->m_reg_aladin->SetNumberOfLevels(std::stoi(criterion.second[0]));
    }
    else
    {
      // TODO log error?
      std::cout << criterion.first << " accepts one number only" << std::endl;
      return false;
    }
  }
  return meetsCriteria;
}

template< class TPixel >
bool
NiftyregAladinComponent<  TPixel >
::ConnectionsSatisfied()
{
  // This function overrides the default behavior, in which all accepting interfaces must be set, by allowing the itkTransformParametersAdaptorsContainerInterface not being set.
  // TODO: see I we can reduce the amount of code with helper (meta-)functions
  if( !this->InterfaceAcceptor< NiftyregReferenceImageInterface< TPixel >>::GetAccepted())
  {
    return false;
  }
  if( !this->InterfaceAcceptor< NiftyregFloatingImageInterface< TPixel >>::GetAccepted() )
  {
    return false;
  }
  return true;
}

} //end namespace selx
