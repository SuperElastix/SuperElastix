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

#ifndef selxItkTransformDataObjectFileReader_hxx
#define selxItkTransformDataObjectFileReader_hxx

#include "selxItkTransformDataObjectFileReader.h"

namespace selx
{
/**
 * ********************* Constructor *********************
 */

template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >
::ItkTransformDataObjectFileReader()
{
  m_Reader = ReaderType::New();
  m_DecoratedTransform = DecoratedTransformType::New();
} // end Constructor


/**
* ********************* Destructor *********************
*/

template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >
::~ItkTransformDataObjectFileReader()
{
} // end Destructor


template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
void
ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >
::SetFileName( const std::string _arg )
{
  return m_Reader->SetFileName( _arg );
}


template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
typename ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >::OutputDataType
* ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >
::GetOutput()
{
  //TODO it is bad design to call Update here. We could consider storing an empty decoratedTransform: (itk::ProcessObject) this->SetOutput(decoratedTransform). THe m_source of this decoratedTransformObject points to this class and might give us a mechanism for updating this object by setting the nakedtransform.
  this->m_Reader->Update();
  auto transformList = this->m_Reader->GetTransformList();
  if (transformList->size() > 1)
  {
    itkExceptionMacro("Reading a transform file containing more than 1 transform is not implemented");
  }
  auto nakedTransformBase = *(transformList->begin());
  const TransformType * nakedTransform = dynamic_cast<TransformType*>(nakedTransformBase.GetPointer());
  assert(nakedTransform != nullptr);
  
  this->m_DecoratedTransform->Set(nakedTransform);

  //implicit cast from DecoratedTransformType<>* to OutputDataType*.
  return this->m_DecoratedTransform;
}

template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
void
ItkTransformDataObjectFileReader< TParametersValueType, NInputDimensions, NOutputDimensions >
::Update()
{
  return m_Reader->Update();
}
} // namespace elx

#endif // selxProcessObject_hxx
