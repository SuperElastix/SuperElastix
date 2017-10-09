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
  m_MiniPipelineOutputImage = DecoratedTransformType::New();
}


template< int Dimensionality, class TInternalComputationValue >
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::~ItkTransformSinkComponent()
{
}


template< int Dimensionality, class TInternalComputationValue >
int
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::Set(typename itkTransformInterface< TInternalComputationValue, Dimensionality >::Pointer other)
{
  // Store pointer to the m_WarpedImageInterface for getting the result image after in has been generated (registration).
  // TODO: sanity check that m_WarpedImageInterface was Null to detect if Set was called more than once erroneously.
  this->m_TransformInterface = other;
  return 0;
}


template< int Dimensionality, class TInternalComputationValue >
void
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::SetMiniPipelineOutput( itk::DataObject::Pointer NetworkBuilderOutput )
{
  /** Tries to cast the NetworkBuilderOutput to an image (data object) and stores the result.
   *  The resulting output image will be grafted into when the sink component is connected to an other component.
   * */
  //
  /*
  this->m_NetworkBuilderOutputImage = dynamic_cast< ItkImageType * >( NetworkBuilderOutput.GetPointer() );
  if( this->m_NetworkBuilderOutputImage == nullptr )
  {
    throw std::runtime_error( "SinkComponent cannot cast the NetworkBuilder's Output to the required type" );
  }
  */
}


template< int Dimensionality, class TInternalComputationValue >
typename itk::DataObject::Pointer
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::GetMiniPipelineOutput()
{
  return this->m_MiniPipelineOutputImage.GetPointer();
}


template< int Dimensionality, class TInternalComputationValue >
typename AnyFileWriter::Pointer
ItkTransformSinkComponent< Dimensionality, TInternalComputationValue >::GetOutputFileWriter()
{
  // Instanstiate an image file writer, decorated such that it can be implicitly cast to an AnyFileWriterType
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
  this->m_MiniPipelineOutputImage->Set(transform);
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
