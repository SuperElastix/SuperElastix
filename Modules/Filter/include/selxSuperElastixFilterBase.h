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

#ifndef selxSuperElastixFilterBase_h
#define selxSuperElastixFilterBase_h

#include "selxBlueprint.h"
#include "selxLogger.h"

#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

/**
 * \class SuperElastixFilterBase
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{
// Forward declaration, hiding implementation details and speeding up compilation time (PIMPL idiom)
class NetworkBuilderBase;
class NetworkBuilderFactoryBase;
class NetworkContainer;

class SuperElastixFilterBase : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef SuperElastixFilterBase          Self;
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

  typedef Blueprint::Pointer       BlueprintPointer;
  typedef Blueprint::ConstPointer  BlueprintConstPointer;
  itkSetObjectMacro( Blueprint, Blueprint );
  itkGetObjectMacro( Blueprint, Blueprint );

  typedef Logger::Pointer        LoggerPointer;
  typedef Logger::ConstPointer   LoggerConstPointer;
  itkSetObjectMacro( Logger, Logger );
  itkGetObjectMacro( Logger, Logger );

  // Adding a BlueprintImpl composes SuperElastixFilter' internal blueprint (accessible by Set/Get BlueprintImpl) with the otherBlueprint.
  // void AddBlueprint(BlueprintPointer otherBlueprint);

  bool ParseBlueprint( void );

  AnyFileReaderType::Pointer GetInputFileReader( const DataObjectIdentifierType & );

  AnyFileWriterType::Pointer GetOutputFileWriter( const DataObjectIdentifierType & );

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  void SetInput(const DataObjectIdentifierType &, InputDataType *) ITK_OVERRIDE;

  /** Non type-specific GetOutput */
  OutputDataType * GetOutput( const DataObjectIdentifierType & );

  /** GetOutput tries dynamic cast to required output type */
  template< typename ReturnType >
  ReturnType * GetOutput( const DataObjectIdentifierType & outputName )
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

    this->Modified();
    return newOutput;
  }

  void Update( void ) ITK_OVERRIDE;

  // The default logger redirects to std::cout 
  void SetLogger( Logger::Pointer logger );

protected:

  // default constructor initialized with an empty NetworkBuilder
  SuperElastixFilterBase( void );

  virtual void GenerateOutputInformation( void ) ITK_OVERRIDE;

  virtual void GenerateData( void ) ITK_OVERRIDE;

  std::unique_ptr< NetworkBuilderFactoryBase > m_NetworkBuilderFactory;
  std::unique_ptr< NetworkBuilderBase >        m_NetworkBuilder;
  std::unique_ptr< NetworkContainer >          m_NetworkContainer;
  LoggerPointer m_Logger;

private:

  BlueprintPointer m_Blueprint;

  bool m_IsConnected;
  bool m_AllUniqueComponents;
};
} // namespace elx

#endif // selxSuperElastixFilterBase_h
