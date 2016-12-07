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

#include "selxItkMeshSource.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkMeshSourceComponent< Dimensionality, TPixel >
::ItkMeshSourceComponent()
{
  this->m_Mesh = nullptr;
}


template< int Dimensionality, class TPixel >
ItkMeshSourceComponent< Dimensionality, TPixel >
::~ItkMeshSourceComponent()
{
}


template< int Dimensionality, class TPixel >
typename ItkMeshSourceComponent< Dimensionality, TPixel >::ItkMeshType::Pointer
ItkMeshSourceComponent< Dimensionality, TPixel >
::GetItkMesh()
{
  if( this->m_Mesh == nullptr )
  {
    itkExceptionMacro( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }
  return this->m_Mesh;
}


template< int Dimensionality, class TPixel >
void
ItkMeshSourceComponent< Dimensionality, TPixel >
::SetMiniPipelineInput( itk::DataObject::Pointer object )
{
  this->m_Mesh = dynamic_cast< ItkMeshType * >( object.GetPointer() );
  if( this->m_Mesh == nullptr )
  {
    itkExceptionMacro( "DataObject passed by the NetworkBuilder is not of the right MeshType or not at all an MeshType" );
  }
  return;
}


template< int Dimensionality, class TPixel >
typename AnyFileReader::Pointer
ItkMeshSourceComponent< Dimensionality, TPixel >::GetInputFileReader()
{
  // Instanstiate an image file reader, decorated such that it can be implicitly cast to an AnyFileReaderType
  return DecoratedReaderType::New().GetPointer();
}


template< int Dimensionality, class TPixel >
bool
ItkMeshSourceComponent< Dimensionality, TPixel >
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
