/*=========================================================================
 *
 *  Copyright UMC Utrecht and contributors
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
#ifndef selxFileWriterDecorator_hxx
#define selxFileWriterDecorator_hxx

#include "selxFileWriterDecorator.h"

namespace selx
{

/**
 * ********************* Constructor *********************
 */

  template< typename TWriter, typename FileWriterDecoratorTraits >
  FileWriterDecorator< TWriter, typename FileWriterDecoratorTraits>
::FileWriterDecorator()
{
  m_Writer = WriterType::New();
} // end Constructor

/**
* ********************* Destructor *********************
*/

  template< typename TWriter, typename FileWriterDecoratorTraits>
  FileWriterDecorator< TWriter, typename FileWriterDecoratorTraits>
::~FileWriterDecorator()
{
} // end Destructor


  template< typename TWriter, typename FileWriterDecoratorTraits>
void
FileWriterDecorator< TWriter, typename FileWriterDecoratorTraits>
::SetFileName(const std::string _arg)
{
  return m_Writer->SetFileName(_arg);
}

template< typename TWriter, typename FileWriterDecoratorTraits>
void
FileWriterDecorator< TWriter, typename FileWriterDecoratorTraits>
::SetInput(const InputDataType* input)
{
  DerivedInputDataType::ConstPointer derivedData = dynamic_cast<const DerivedInputDataType*>(input);	
  if (derivedData==nullptr)
  {
      itkExceptionMacro("The AnyFileWriter redirects to a FileWriterDecorator with a WriterType not compatible with the input data");
  }

  return m_Writer->SetInput(derivedData);
}

template< typename TWriter, typename FileWriterDecoratorTraits>
void
FileWriterDecorator< TWriter, typename FileWriterDecoratorTraits>
::Update()
{
  return m_Writer->Update();
}

} // namespace elx

#endif // selxProcessObject_hxx
