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
#ifndef selxFileReaderDecorator_hxx
#define selxFileReaderDecorator_hxx

#include "selxFileReaderDecorator.h"

namespace selx
{

/**
 * ********************* Constructor *********************
 */

template< typename TReader >
FileReaderDecorator< TReader >
::FileReaderDecorator(void)
{
  m_Reader = ReaderType::New();
} // end Constructor

/**
* ********************* Destructor *********************
*/

template< typename TReader >
FileReaderDecorator< TReader >
::~FileReaderDecorator(void)
{
} // end Destructor


template< typename TReader >
void
FileReaderDecorator< TReader >
::SetFileName(const std::string _arg)
{
  return m_Reader->SetFileName(_arg);
}

template< typename TReader >
typename FileReaderDecorator< TReader >::OutputDataType*
FileReaderDecorator< TReader >
::GetOutput()
{
  //implicit cast from ImageType<>* to OutputDataType*.
  return m_Reader->GetOutput();
}

template< typename TReader >
void
FileReaderDecorator< TReader >
::Update()
{
  return m_Reader->Update();
}

} // namespace elx

#endif // selxProcessObject_hxx
