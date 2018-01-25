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

#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
::ItkDisplacementFieldSourceComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger ), m_DisplacementField( nullptr )
{
}


template< int Dimensionality, class TPixel >
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
::~ItkDisplacementFieldSourceComponent()
{
}


template< int Dimensionality, class TPixel >
typename ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >::ItkDisplacementFieldType::Pointer
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
::GetItkDisplacementField()
{
  if( this->m_DisplacementField == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }
  return this->m_DisplacementField;
}


template< int Dimensionality, class TPixel >
typename ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >::ItkImageDomainPointer
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
::GetItkImageDomainFixed()
{
  if( this->m_DisplacementField == nullptr )
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }
  return m_DisplacementField.GetPointer();
}


template< int Dimensionality, class TPixel >
void
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
::SetMiniPipelineInput( itk::DataObject::Pointer object )
{
  this->m_DisplacementField = dynamic_cast< ItkDisplacementFieldType * >( object.GetPointer() );
  if( this->m_DisplacementField == nullptr )
  {
    throw std::runtime_error( "DataObject passed by the NetworkBuilder is not of the right DisplacementFieldType or not at all an DisplacementFieldType" );
  }
  return;
}


template< int Dimensionality, class TPixel>
typename AnyFileReader::Pointer
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >::GetInputFileReader()
{
  // Instanstiate an image file reader, decorated such that it can be implicitly cast to an AnyFileReaderType
  return DecoratedReaderType::New().GetPointer();
}


template< int Dimensionality, class TPixel >
bool
ItkDisplacementFieldSourceComponent< Dimensionality, TPixel >
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
