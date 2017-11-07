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

#include "selxItkTransformSinkComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TInternalComputationValue >
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::ItkTransformSinkComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name,
    logger )
{
  m_MiniPipelineOutput = DecoratedTransformType::New();
}


template< int Dimensionality, class TInternalComputationValue >
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::~ItkTransformSinkComponent()
{
}


template< int Dimensionality, class TInternalComputationValue >
int
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::Accept(typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer other)
{
  // Store pointer to the m_TransformInterface for getting the result transform after in has been generated (registration).
  // TODO: sanity check that m_TransformInterface was Null to detect if Set was called more than once erroneously.
  this->m_TransformInterface = other;
  return 0;
}


template< int Dimensionality, class TInternalComputationValue >
void
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::SetMiniPipelineOutput( itk::DataObject::Pointer NetworkBuilderOutput )
{
// deprecated
}


template< int Dimensionality, class TInternalComputationValue >
typename itk::DataObject::Pointer
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::GetMiniPipelineOutput()
{
  return this->m_MiniPipelineOutput.GetPointer();
}


template< int Dimensionality, class TInternalComputationValue >
typename AnyFileWriter::Pointer
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::GetOutputFileWriter()
{
  // Instanstiate an transform file writer, decorated such that it can be implicitly cast to an AnyFileWriterType
  return DecoratedWriterType::New().GetPointer();
}


template< int Dimensionality, class TInternalComputationValue >
typename itk::DataObject::Pointer
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::GetInitializedOutput()
{
  return DecoratedTransformType::New().GetPointer();
}

template< int Dimensionality, class TInternalComputationValue >
void
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::AfterRegistration()
{
  // Only after having performed the registration we can get the pointer to the result transform.
  auto transform = this->m_TransformInterface->GetItkTransform();  
  // This pointer is put into the DataObjectDecorater (which was empty before) such that is can be passed in an itk-pipeline.
  this->m_MiniPipelineOutput->Set(transform);
}

template< int Dimensionality, class TInternalComputationValue >
bool
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
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
