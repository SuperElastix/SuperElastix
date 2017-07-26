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

#ifndef selxItkTransfromDataObjectFileWriter_h
#define selxItkTransfromDataObjectFileWriter_h

#include "selxAnyFileWriter.h"
#include "itkTransformFileWriter.h"


/**
 * \class selxItkTransfromDataObjectFileWriter
 * \brief Wrapper class, for a template specifiable Writer, that can be casted to an AnyFileWriter base class.
 */

namespace selx
{
template< typename TParametersValueType, int NInputDimensions, int NOutputDimensions >
class ItkTransfromDataObjectFileWriter : public AnyFileWriter
{
public:

  /** Standard ITK typedefs. */
  typedef ItkTransfromDataObjectFileWriter             Self;
  typedef AnyFileWriter                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  typedef itk::TransformFileWriterTemplate<TParametersValueType>                        WriterType;
  typedef typename WriterType::Pointer      WriterPointer;
  typedef typename WriterType::ConstPointer WriterConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, selxAnyFileWriter );
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  // Since we don't know the typename of the Input of the writer, we use a traits class with template specialization to Images, Meshes etc.
  //typedef typename WriterType::InputImageType DerivedInputDataType;
  //typedef typename WriterType::InputMeshType DerivedInputDataType;
  typedef typename itk::DataObjectDecorator<itk::Transform<TParametersValueType,NInputDimensions,NOutputDimensions>> DerivedInputDataType;

  virtual void SetFileName( const std::string ) ITK_OVERRIDE;

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  virtual void SetInput( const InputDataType * ) ITK_OVERRIDE;

  virtual void Update( void ) ITK_OVERRIDE;

  ItkTransfromDataObjectFileWriter( void );
  ~ItkTransfromDataObjectFileWriter( void );

protected:

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;

private:

  // the actual itk writer instantiation
  WriterPointer m_Writer;
  typename DerivedInputDataType::ConstPointer m_DerivedData;
};
} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkTransfromDataObjectFileWriter.hxx"
#endif

#endif // selxProcessObject_h
