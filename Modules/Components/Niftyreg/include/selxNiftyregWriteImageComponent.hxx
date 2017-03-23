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

#include "selxNiftyregWriteImageComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< class TPixel >
NiftyregWriteImageComponent< TPixel >::NiftyregWriteImageComponent( const std::string & name ) : Superclass( name )
{
}


template< class TPixel >
NiftyregWriteImageComponent< TPixel >::~NiftyregWriteImageComponent()
{
}

template< class TPixel >
int NiftyregWriteImageComponent<  TPixel >::Set(typename NiftyregWarpedImageInterface< TPixel >::Pointer component)
{
  this->m_WarpedImageInterface = component;
  return 1;
}

template< class TPixel >
void
NiftyregWriteImageComponent<  TPixel >::AfterRegistration()
{
  auto warpedImage = this->m_WarpedImageInterface->GetWarpedNiftiImage();
  reg_io_WriteImageFile(warpedImage.get(), this->m_ImageFileName.c_str());
}


template< class TPixel >
bool
NiftyregWriteImageComponent<  TPixel >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
