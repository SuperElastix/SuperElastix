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

#ifndef selxItkTransfromDataObjectFileWriter_hxx
#define selxItkTransfromDataObjectFileWriter_hxx

#include "selxItkTransfromDataObjectFileWriter.h"

namespace selx
{
/**
 * ********************* Constructor *********************
 */

template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
ItkTransfromDataObjectFileWriter< TParametersValueType, NInputDimensions, NOutputDimensions >
::ItkTransfromDataObjectFileWriter()
{
  m_Writer = WriterType::New();
} // end Constructor


/**
* ********************* Destructor *********************
*/

template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
ItkTransfromDataObjectFileWriter< TParametersValueType, NInputDimensions, NOutputDimensions >
::~ItkTransfromDataObjectFileWriter()
{
} // end Destructor


template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
void
ItkTransfromDataObjectFileWriter< TParametersValueType, NInputDimensions, NOutputDimensions >
::SetFileName( const std::string _arg )
{
  return m_Writer->SetFileName( _arg );
}


template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
void
ItkTransfromDataObjectFileWriter< TParametersValueType, NInputDimensions, NOutputDimensions >
::SetInput( const InputDataType * input )
{
  this->m_DerivedData = dynamic_cast< const DerivedInputDataType * >( input );
  if (this->m_DerivedData == nullptr)
  {
    itkExceptionMacro( "The AnyFileWriter redirects to a ItkTransfromDataObjectFileWriter with a WriterType not compatible with the input data" );
  }
}


template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
void
ItkTransfromDataObjectFileWriter< TParametersValueType, NInputDimensions, NOutputDimensions >
::Update()
{
  this->m_Writer->SetInput(this->m_DerivedData->Get());
  return this->m_Writer->Update();
}
} // namespace elx

#endif // selxProcessObject_hxx
