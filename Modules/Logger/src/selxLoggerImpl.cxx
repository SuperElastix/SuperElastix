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

#include "selxLoggerImpl.h"

namespace selx
{

LoggerImpl
::LoggerImpl() : m_Loggers(), m_QueueSize( 262144 ), m_OverflowPolicy( spdlog::async_overflow_policy::block_retry )
{
  this->SetSyncMode();
  this->SetFormat( "[%Y-%m-%d %H:%M:%S] [thread %t] [%l] %v" );
}

LoggerImpl
::~LoggerImpl()
{
  spdlog::drop_all();
}

void
LoggerImpl
::SetLogLevel( const LogLevel& level ) {
  switch (level) {
    case LogLevel::TRACE:
      this->SetLogLevel( spdlog::level::level_enum::trace );
      break;
    case LogLevel::DEBUG:
      this->SetLogLevel( spdlog::level::level_enum::debug );
      break;
    case LogLevel::INFO:
      this->SetLogLevel( spdlog::level::level_enum::info );
      break;
    case LogLevel::WARNING:
      this->SetLogLevel( spdlog::level::level_enum::warn );
      break;
    case LogLevel::ERROR:
      this->SetLogLevel( spdlog::level::level_enum::err );
      break;
    case LogLevel::CRITICAL:
      this->SetLogLevel( spdlog::level::level_enum::critical );
      break;
    case LogLevel::OFF:
      this->SetLogLevel( spdlog::level::level_enum::off );
      break;
    default:
      itkGenericExceptionMacro( "Invalid log level." );
  }
}

void
LoggerImpl
::SetLogLevel( const spdlog::level::level_enum& level ) {
  spdlog::set_level( level );
  std::for_each(this->m_Loggers.begin(), this->m_Loggers.end(), [ level ]( LoggerType logger )
  {
    logger->set_level( level );
    if ( "err" != logger->name() ) {
      logger->set_level( level );
    } else {
      // Only change err log level if it is 'trace' or 'debug'
      if ( level <= spdlog::level::debug ) {
        logger->set_level( level );
      }
    }
  } );
}


void
LoggerImpl
::SetFormat( const std::string& format )
{
  spdlog::set_pattern( format );
}

void
LoggerImpl
::SetSyncMode()
{
  spdlog::set_sync_mode();
}

void
LoggerImpl
::SetAsyncMode()
{
  spdlog::set_async_mode(this->m_QueueSize, this->m_OverflowPolicy);
}

void
LoggerImpl
::SetAsyncBlockOnOverflow( void )
{
  this->m_OverflowPolicy = OverflowPolicyType::block_retry;
  this->SetAsyncMode();
}

void
LoggerImpl
::SetAsyncDiscardOnOverflow( void )
{
  this->m_OverflowPolicy = OverflowPolicyType::discard_log_msg;
  this->SetAsyncMode();
}

void
LoggerImpl
::SetAsyncQueueSize( const size_t& queueSize )
{
  this->m_QueueSize = queueSize;
  this->SetAsyncMode();
}

void
LoggerImpl
::Trace( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->trace( message );
  }
}

void
LoggerImpl
::Debug( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->debug( message );
  }
}

void
LoggerImpl
::Info( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->info( message );
  }
}

void
LoggerImpl
::Warning( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->warn( message );
  }
}

void
LoggerImpl
::Error( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->error( message );
  }
}

void
LoggerImpl
::Critical( const std::string& message )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->critical( message );
  }
}


void
LoggerImpl
::AddOutLogger( void )
{
  auto logger = spdlog::stdout_logger_mt( "cout" );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddOutLoggerWithColors( void )
{
  auto logger = spdlog::stdout_color_mt( "cout" );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddErrLogger( void )
{
  auto logger = spdlog::stderr_logger_mt( "err" );
  logger->set_level( spdlog::level::debug );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddErrLoggerWithColors( void )
{
  auto logger = spdlog::stderr_color_mt( "err" );
  logger->set_level( spdlog::level::debug );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddFileLogger( const std::string& fileName )
{
  auto logger = spdlog::basic_logger_mt( fileName, fileName );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddDailyFileLogger( const std::string& fileName, const int& hour, const int& minute )
{
  auto logger = spdlog::daily_logger_mt( fileName, fileName, hour, minute );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize, const size_t& maxNumberOfFiles )
{
  auto logger = spdlog::rotating_logger_mt( fileName, fileName, maxFileSize, maxNumberOfFiles );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::RemoveLogger( const std::string& name )
{
  spdlog::drop( name );
}


} // namespace
