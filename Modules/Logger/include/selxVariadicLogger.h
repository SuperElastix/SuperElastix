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
#ifndef selxVariadicLogger_h
#define selxVariadicLogger_h

#include "spdlog/spdlog.h"

#include "selxComponentLogger.h"

#include "itkDataObject.h"
#include "itkObjectFactory.h"

namespace selx
{

class VariadicLogger : public itk::DataObject, ComponentLogger
{
public:

  /** Standard ITK typedefs. */
  typedef VariadicLogger                          Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::DataObject );

  /* m_LoggerImpl is initialized in the default constructor */
  VariadicLogger();
  ~VariadicLogger();

  using LogLevel = spdlog::level::level_enum;
  typedef std::shared_ptr< spdlog::logger > LoggerType;
  typedef std::vector< LoggerType > LoggerVectorType;
  typedef spdlog::async_overflow_policy OverflowPolicyType;

  void SetLogLevel( const LogLevel& level );
  void SetPattern( const std::string& format );

  void SetSyncMode();
  void SetAsyncMode();
  void SetAsyncBlockOnOverflow( void );
  void SetAsyncDiscardOnOverflow( void );
  void SetAsyncQueueSize( const size_t& queueSize );

  template <typename... Args> void Trace( const char* fmt, const Args&... args );
  template <typename... Args> void Debug( const char* fmt, const Args&... args );
  template <typename... Args> void Info( const char* fmt, const Args&... args );
  template <typename... Args> void Error( const char* fmt, const Args&... args );
  template <typename... Args> void Warning( const char* fmt, const Args&... args );
  template <typename... Args> void Critical( const char* fmt, const Args&... args );

  void AddOutLogger( void ); // Remove with name "out"
  void AddOutLoggerWithColors( void ); // Remove with name "err"
  void AddErrLogger( void ); // Remove with name "out"
  void AddErrLoggerWithColors( void ); // Remove with name "err"
  void AddFileLogger( const std::string& fileName );
  void AddDailyFileLogger( const std::string& fileName, const int& hours = 0, const int& minutes = 0 );
  void AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize = 262144, const size_t& maxNumberOfFiles = 1);
  void RemoveLogger( const std::string& fileName );

private:
  size_t m_QueueSize;
  OverflowPolicyType m_OverflowPolicy;
  LoggerVectorType m_Loggers;

};
} // namespace

#include "selxVariadicLogger.hxx"

#endif // selxVariadicLogger_h
