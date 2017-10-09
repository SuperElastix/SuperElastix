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

#ifndef selxLoggerImpl_h
#define selxLoggerImpl_h

#include "selxLogger.h"
#include "spdlog/spdlog.h"

namespace selx
{

class LoggerImpl
{
public:

  typedef std::shared_ptr< spdlog::logger > LoggerType;
  typedef std::vector< LoggerType > LoggerVectorType;
  typedef spdlog::async_overflow_policy OverflowPolicyType;

  LoggerImpl();
  ~LoggerImpl();

  void SetLogLevel( const LogLevel& level );
  void SetLogLevel( const spdlog::level::level_enum& level );
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

  void AddOutLogger( void );
  void AddOutLoggerWithColors( void );
  void AddErrLogger( void );
  void AddErrLoggerWithColors( void );
  void AddFileLogger( const std::string& fileName );
  void AddDailyFileLogger( const std::string& fileName, const int& hour, const int& minute);
  void AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize, const size_t& maxNumberOfFiles);
  void RemoveLogger( const std::string& name );

private:

  size_t m_QueueSize;
  OverflowPolicyType m_OverflowPolicy;
  LoggerVectorType m_Loggers;
};
}

#endif // selxLoggerImpl_h
