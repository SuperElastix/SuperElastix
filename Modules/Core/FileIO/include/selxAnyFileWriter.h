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

#ifndef AnyFileWriter_h
#define AnyFileWriter_h

#include "itkProcessObject.h"

/**
* \class AnyFileWriter
* \brief A base class, without template arguments, to handle any file Writer that derives from this class.
*/

namespace selx
{
class AnyFileWriter : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef AnyFileWriter                   Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::ProcessObject );
  typedef itk::DataObject InputDataType;

  virtual void SetFileName( const std::string ) = 0;

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  virtual void SetInput( const InputDataType * ) = 0;

  virtual void Update( void ) = 0;

protected:

  //AnyFileWriter(void) {};
  //~AnyFileWriter(void) {};

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;
};
} // namespace elx

#endif // AnyFileWriter_h
