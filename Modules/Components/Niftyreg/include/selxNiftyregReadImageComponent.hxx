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

#include "selxNiftyregReadImageComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
NiftyregReadImageComponent< TPixel >::NiftyregReadImageComponent()
{
}

template< class TPixel >
NiftyregReadImageComponent< TPixel >::~NiftyregReadImageComponent()
{
}

template< class TPixel >
nifti_image *
NiftyregReadImageComponent<  TPixel >::GetReferenceNiftiImage( )
{
    return reg_io_ReadImageFile(this->m_ImageFileName.c_str());
}

template< class TPixel >
nifti_image *
NiftyregReadImageComponent<  TPixel >::GetFloatingNiftiImage()
{
    return reg_io_ReadImageFile(this->m_ImageFileName.c_str());
}


template< class TPixel >
bool
NiftyregReadImageComponent<  TPixel >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
  else if( criterion.first == "FileName" ) //Supports this?
  {
    meetsCriteria = true;
    if( criterion.second.size() == 1 )
    {
        //TODO check if file exists
        this->m_ImageFileName = criterion.second[ 0 ];

    }
  }
  return meetsCriteria;
}

} //end namespace selx 
