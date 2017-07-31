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

#include "selxItkTransformSourceComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class InternalComputationValueType >
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::ItkTransformSourceComponent(const std::string & name, const LoggerInterface & logger) : Superclass(name,
  logger)
{
}


template< int Dimensionality, class InternalComputationValueType >
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::~ItkTransformSourceComponent()
{
}


template< int Dimensionality, class InternalComputationValueType >
typename ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::TransformPointer
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::GetItkTransform()
{
  if (this->m_DecoratedTransform == nullptr)
  {
    throw std::runtime_error( "SourceComponent needs to be initialized by SetMiniPipelineInput()" );
  }
  return this->m_DecoratedTransform->GetModifiable();
}


template< int Dimensionality, class InternalComputationValueType >
void
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >
::SetMiniPipelineInput( itk::DataObject::Pointer object )
{
  this->m_DecoratedTransform = dynamic_cast< DecoratedTransformType * >(object.GetPointer());
  if (this->m_DecoratedTransform == nullptr)
  {
    throw std::runtime_error( "DataObject passed by the NetworkBuilder is not of the right DecoratedTransformType or not at all an DecoratedTransformType" );
  }
  return;
}


template< int Dimensionality, class InternalComputationValueType >
typename AnyFileReader::Pointer
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::GetInputFileReader()
{
  // Instanstiate an image file reader, decorated such that it can be implicitly cast to an AnyFileReaderType
  return DecoratedReaderType::New().GetPointer();
}



template< int Dimensionality, class InternalComputationValueType >
bool
ItkTransformSourceComponent< Dimensionality, InternalComputationValueType >::MeetsCriterion(const ComponentBase::CriterionType & criterion)
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
