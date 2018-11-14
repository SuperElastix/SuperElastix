
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

#ifndef selxDisplacementFieldMeshWarperComponent_hxx
#define selxDisplacementFieldMeshWarperComponent_hxx

#include "selxDisplacementFieldMeshWarperComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >
::ItkDisplacementFieldMeshWarperComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger ) {
  this->m_DisplacementFieldTransform = ItkDisplacementFieldTransformType::New();
  this->m_TransformMeshFilter = ItkTransformMeshFilterType::New();
  this->m_TransformMeshFilter->SetTransform( this->m_DisplacementFieldTransform );
};

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >
::Accept( ItkDisplacementFieldInterfacePointer itkDisplacementFieldInterface )
{
  auto displacementField = itkDisplacementFieldInterface->GetItkDisplacementField();
  displacementField->Update(); // Needed to load world info before transform is run
  displacementField->SetBufferedRegion(displacementField->GetRequestedRegion());
  this->m_DisplacementFieldTransform->SetDisplacementField( displacementField );

  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >
::Accept( ItkMeshInterfacePointer itkMeshInterface )
{
  this->m_TransformMeshFilter->SetInput( itkMeshInterface->GetItkMesh() );

  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
typename ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >::ItkMeshType::Pointer
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >
::GetItkMesh()
{
  return this->m_TransformMeshFilter->GetOutput();
}

template< int Dimensionality, class TPixel, class CoordRepType >
bool
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType >
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

} // namespace selx

#endif // selxDisplacementFieldMeshWarperComponent_hxx