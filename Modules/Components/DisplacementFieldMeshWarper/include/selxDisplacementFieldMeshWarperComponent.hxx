
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

#ifndef selxDisplacementFieldWarperComponent_hxx
#define selxDisplacementFieldWarperComponent_hxx

#include "selxDisplacementFieldMeshWarperComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx {

template< int Dimensionality, class TPixel, class TCoordRepType >
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >
::ItkDisplacementFieldMeshWarperComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger ) {
  this->m_DisplacementFieldTransform = ItkDisplacementFieldTransformType::New();
  this->m_TransformMeshFilter = ItkTransformMeshFilterType::New();
  this->m_TransformMeshFilter->SetTransform( this->m_DisplacementFieldTransform );
};

template< int Dimensionality, class TPixel, class TCoordRepType >
int
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >
::Accept( ItkDisplacementFieldInterfacePointer itkDisplacementFieldInterface )
{
  auto displacementField = itkDisplacementFieldInterface->GetItkDisplacementField();
  this->m_DisplacementFieldTransform->SetDisplacementField( displacementField );

  return 0;
}

template< int Dimensionality, class TPixel, class TCoordRepType >
int
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >
::Accept( ItkMeshInterfacePointer itkMeshInterface )
{
  this->m_TransformMeshFilter->SetInput( itkMeshInterface->GetItkMesh() );

  return 0;
}

template< int Dimensionality, class TPixel, class TCoordRepType >
typename ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >::ItkMeshType::Pointer
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >
::GetItkMesh()
{
  this->m_TransformMeshFilter->Update();
  return itkDynamicCastInDebugMode< ItkMeshType * >(this->m_TransformMeshFilter->GetOutput());
}

template< int Dimensionality, class TPixel, class TCoordRepType >
bool
ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType >
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

#endif // selxDisplacementFieldWarperComponent_hxx