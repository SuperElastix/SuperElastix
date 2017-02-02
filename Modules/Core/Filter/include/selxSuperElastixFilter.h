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

#ifndef selxSuperElastixFilter_h
#define selxSuperElastixFilter_h

#include "itkProcessObject.h"
#include "selxBlueprint.h"

#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"
//#include "itkSharedPointerDataObjectDecorator.h"
#include "itkUniquePointerDataObjectDecorator.h"

/**
 * \class SuperElastixFilter
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{
class NetworkBuilderBase; // forward declaration, hiding implementation details and speeding up compilation time (PIMPL idiom)
  
class SuperElastixFilter : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef SuperElastixFilter              Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::ProcessObject );
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  typedef AnyFileReader AnyFileReaderType;
  typedef AnyFileWriter AnyFileWriterType;

  typedef itk::UniquePointerDataObjectDecorator< Blueprint > BlueprintType;
  typedef BlueprintType::Pointer                                      BlueprintPointer;
  typedef BlueprintType::ConstPointer                                 BlueprintConstPointer;

  // TODO: Make const-correct
  itkSetObjectMacro( Blueprint, BlueprintType )

  AnyFileReaderType::Pointer GetInputFileReader( const DataObjectIdentifierType & );

  AnyFileWriterType::Pointer GetOutputFileWriter( const DataObjectIdentifierType & );

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  void SetInput( const DataObjectIdentifierType &, InputDataType * );

  /** Non type-specific GetOutput */
  OutputDataType * GetOutput( const DataObjectIdentifierType & );

  /** GetOutput tries dynamic cast to required output type */
  template< typename ReturnType >
  ReturnType * GetOutput( const DataObjectIdentifierType & outputName)
  {
      OutputDataType * output = Superclass::GetOutput( outputName );
  if( output != nullptr ) // if an output already exists, return it
  {
    ReturnType * returnOutput = dynamic_cast< ReturnType * >( output );
    if( returnOutput != nullptr ) // if it is of the same type as requested before
    {
      return returnOutput;
    }
    itkExceptionMacro( << "Output " "" << outputName << "" " was requested before, but the ReturnTypes do not match" )
  }
  // Purposely not checking the outputName, but just create the requested&named data object in the filter.
  // When connecting the Sinks the selxFilter names and data types are checked.
  typename ReturnType::Pointer newOutput = ReturnType::New();

  Superclass::SetOutput( outputName, newOutput );

  this->m_OutputConnectionModified = true;
  return newOutput;
  };

  void Update( void ) ITK_OVERRIDE;

protected:
  // default constructor for API library use
  SuperElastixFilter( void );

  // special constructor which is called by the templated SuperElastixFilter for SDK library use
  SuperElastixFilter( bool InitializeEmptyComponentList );

  virtual void GenerateOutputInformation( void ) ITK_OVERRIDE;

  virtual void GenerateData( void ) ITK_OVERRIDE;

  std::unique_ptr< NetworkBuilderBase >   m_NetworkBuilder;

private:

  //TODO make const correct
  BlueprintType::Pointer      m_Blueprint;
  
  bool                        m_InputConnectionModified;
  bool                        m_OutputConnectionModified;
  bool                        m_BlueprintConnectionModified;
  bool                        m_IsConnected;
  bool                        m_AllUniqueComponents;
};
} // namespace elx

#endif // selxSuperElastixFilter_h
