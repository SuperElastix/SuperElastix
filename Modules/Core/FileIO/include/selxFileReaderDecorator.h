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

#ifndef selxFileReaderDecorator_h
#define selxFileReaderDecorator_h

#include "selxAnyFileReader.h"

/**
 * \class selxFileReaderDecorator
 * \brief Wrapper class, for a template specifiable reader, that can be casted to an AnyFileReader base class.
 */

namespace selx
{
template< typename TReader >
class FileReaderDecorator : public AnyFileReader
{
public:

  /** Standard ITK typedefs. */
  typedef FileReaderDecorator             Self;
  typedef AnyFileReader                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  typedef TReader                        ReaderType;
  typedef typename TReader::Pointer      ReaderPointer;
  typedef typename TReader::ConstPointer ReaderConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, selxAnyFileReader );
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  virtual void SetFileName( const std::string ) ITK_OVERRIDE;

  /** The AnyFileReader has a non type-specific, but derived from OutputDataType, GetOutput */
  virtual OutputDataType * GetOutput() ITK_OVERRIDE;

  virtual void Update( void ) ITK_OVERRIDE;

  FileReaderDecorator();
  ~FileReaderDecorator();

protected:

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;

private:

  // the actual itk reader instantiation
  ReaderPointer m_Reader;
};
} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxFileReaderDecorator.hxx"
#endif

#endif // selxProcessObject_h
