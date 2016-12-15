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

#include "selxItkMeshSink.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkMeshSinkComponent< Dimensionality, TPixel >::ItkMeshSinkComponent() :
  m_MiniPipelineOutputMesh( nullptr ), m_NetworkBuilderOutputMesh( nullptr )
{
}


template< int Dimensionality, class TPixel >
ItkMeshSinkComponent< Dimensionality, TPixel >::~ItkMeshSinkComponent()
{
}


template< int Dimensionality, class TPixel >
int
ItkMeshSinkComponent< Dimensionality, TPixel >::Set( itkMeshInterface< Dimensionality, TPixel > * other )
{
  if( this->m_NetworkBuilderOutputMesh == nullptr )
  {
    itkExceptionMacro( "SinkComponent needs to be initialized by ConnectToNetworkBuilderSink()" );
  }

  // Store pointer to MiniPipelineOutputMesh for later grafting onto NetworkBuilder output.
  this->m_MiniPipelineOutputMesh = other->GetItkMesh();
  // Graft NetworkBuilder output onto MiniPipelineOutputMesh.
  // this->m_MiniPipelineOutputMesh->Graft(this->m_NetworkBuilderOutputMesh);
  return 0;
}


template< int Dimensionality, class TPixel >
void
ItkMeshSinkComponent< Dimensionality, TPixel >::AfterRegistration()
{
  this->m_MiniPipelineOutputMesh->Update();
}


template< int Dimensionality, class TPixel >
typename AnyFileWriter::Pointer
ItkMeshSinkComponent< Dimensionality, TPixel >::GetOutputFileWriter()
{
  // Instanstiate an image file writer, decorated such that it can be implicitly cast to an AnyFileWriterType
  return DecoratedWriterType::New().GetPointer();
}


template< int Dimensionality, class TPixel >
void
ItkMeshSinkComponent< Dimensionality, TPixel >::SetMiniPipelineOutput( itk::DataObject::Pointer NetworkBuilderOutput )
{
  /** Tries to cast the NetworkBuilderOutput to an Mesh (data object) and stores the result.
   *  The resulting output Mesh will be grafted into when the sink component is connected to an other component.
   * */
  //
  this->m_NetworkBuilderOutputMesh = dynamic_cast< ItkMeshType * >( &( *NetworkBuilderOutput ) );
  if( this->m_NetworkBuilderOutputMesh == nullptr )
  {
    itkExceptionMacro( "SinkComponent cannot cast the NetworkBuilder's Output to the required type" );
  }
}


template< int Dimensionality, class TPixel >
typename itk::DataObject::Pointer
ItkMeshSinkComponent< Dimensionality, TPixel >::GetMiniPipelineOutput()
{
  return this->m_MiniPipelineOutputMesh.GetPointer();
}


//template<int Dimensionality, class TPixel>
//bool ItkMeshSinkComponent< Dimensionality, TPixel>::ConnectToNetworkBuilderSink(itk::DataObject::Pointer object)
//{
//  this->m_Mesh = dynamic_cast<ItkMeshType*>(&(*object));

//  return (this->m_Mesh != nullptr);
//}
template< int Dimensionality, class TPixel >
typename itk::DataObject::Pointer
ItkMeshSinkComponent< Dimensionality, TPixel >::GetInitializedOutput()
{
  return ItkMeshType::New().GetPointer();
}


template< int Dimensionality, class TPixel >
bool
ItkMeshSinkComponent< Dimensionality, TPixel >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
