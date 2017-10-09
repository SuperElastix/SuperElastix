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

// There already is an sitkLogger preprocessor definition in ITK
#ifndef selxLogger_h
#define selxLogger_h

#include <string>
#include <memory>

#include "itkDataObject.h"
#include "itkObjectFactory.h"

namespace selx
{

typedef enum {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4,
  CRITICAL = 5,
  OFF = 6,
} LogLevel;

class LoggerImpl;

class Logger : public itk::DataObject
{
public:

  /** Standard ITK typedefs. */
  typedef Logger                          Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::DataObject );

  /* m_LoggerImpl is initialized in the default constructor */
  Logger();

  /** The actual logger is a pimpled member variable */
  typedef std::unique_ptr< LoggerImpl > LoggerImplPointer;

  /* Provide a reference to internal logger */
  LoggerImpl & GetLogger( void );

  void SetLogLevel( const LogLevel& level );
  void SetFormat( const std::string& format );

  void SetSyncMode();
  void SetAsyncMode();
  void SetAsyncBlockOnOverflow( void );
  void SetAsyncDiscardOnOverflow( void );
  void SetAsyncQueueSize( const size_t& queueSize );

  void Trace( const std::string& message );
  void Debug( const std::string& message );
  void Info( const std::string& message );
  void Warning( const std::string& message );
  void Error( const std::string& message );
  void Critical( const std::string& message );

  void AddOutLogger( void ); // Remove with name "out"
  void AddOutLoggerWithColors( void ); // Remove with name "err"
  void AddErrLogger( void ); // Remove with name "out"
  void AddErrLoggerWithColors( void ); // Remove with name "err"
  void AddFileLogger( const std::string& fileName );
  void AddDailyFileLogger( const std::string& fileName, const int& hours = 0, const int& minutes = 0 );
  void AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize = 262144, const size_t& maxNumberOfFiles = 1);
  void RemoveLogger( const std::string& fileName );

private:

  LoggerImplPointer m_LoggerImpl;
};
} // namespace

#endif // selxLogger_h
