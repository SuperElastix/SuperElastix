
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

#ifndef selxDisplacementFieldComposerComponent_hxx
#define selxDisplacementFieldComposerComponent_hxx

#include "selxDisplacementFieldComposerComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::ItkDisplacementFieldComposerComponent( const std::string& name, LoggerImpl& logger ) : Superclass( name, logger ) {
  this->m_ComposeDisplacementFieldsImageFilter = ComposeDisplacementFieldsImageFilterType::New();
};

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::Accept( DisplacementFieldInterfacePointer itkDisplacementFieldInterface )
{
  this->m_DisplacementField = itkDisplacementFieldInterface->GetItkDisplacementField();
  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::Accept( WarpingDisplacementFieldInterfacePointer itkWarpingDisplacementFieldInterface )
{
  this->m_WarpingDisplacementField = itkWarpingDisplacementFieldInterface->GetItkDisplacementField();
  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
void
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::BeforeUpdate()
{

  this->m_DisplacementField->Update();
  this->m_WarpingDisplacementField->Update();

  // TODO: By default, the output region is copied to all inputs. Find out where it happens,
  // and disable this behaviour

  this->m_ComposeDisplacementFieldsImageFilter->SetDisplacementField( this->m_DisplacementField );
  this->m_ComposeDisplacementFieldsImageFilter->SetWarpingField( this->m_WarpingDisplacementField );
}


template< int Dimensionality, class TPixel, class CoordRepType >
typename ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >::DisplacementFieldPointer
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::GetItkDisplacementField()
{
  return this->m_ComposeDisplacementFieldsImageFilter->GetOutput();
}

template< int Dimensionality, class TPixel, class CoordRepType >
bool
ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool meetsCriteria( false );
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }

  return meetsCriteria;
}

} // namespace selx

#endif // selxDisplacementFieldComposerComponent_hxx