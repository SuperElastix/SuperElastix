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

#include "selxVariadicLogger.h"

namespace selx
{

VariadicLogger
::VariadicLogger() : m_Loggers(), m_QueueSize( 262144 ), m_OverflowPolicy( spdlog::async_overflow_policy::block_retry )
{
  this->SetSyncMode();
  this->SetPattern( "[%Y-%m-%d %H:%M:%S.%f ] [thread %t] [%l] %v" );
}

VariadicLogger
::~VariadicLogger()
{
  spdlog::drop_all();
}

void
VariadicLogger
::SetLogLevel( const spdlog::level::level_enum& level ) {
  spdlog::set_level( level );
  for( const auto& logger : this->m_Loggers )
  {
    logger->set_level( level );
  }
}


void
VariadicLogger
::SetPattern( const std::string& format )
{
  spdlog::set_pattern( format );
}

void
VariadicLogger
::SetSyncMode()
{
  spdlog::set_sync_mode();
}

void
VariadicLogger
::SetAsyncMode()
{
  spdlog::set_async_mode(this->m_QueueSize, this->m_OverflowPolicy);
}

void
VariadicLogger
::SetAsyncBlockOnOverflow( void )
{
  this->m_OverflowPolicy = OverflowPolicyType::block_retry;
  this->SetAsyncMode();
}

void
VariadicLogger
::SetAsyncDiscardOnOverflow( void )
{
  this->m_OverflowPolicy = OverflowPolicyType::discard_log_msg;
  this->SetAsyncMode();
}

void
VariadicLogger
::Flush( void )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->flush();
  }
}

void
VariadicLogger
::AddOutLogger( void )
{
  auto logger = spdlog::stdout_logger_mt( "cout" );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddOutLoggerWithColors( void )
{
  auto logger = spdlog::stdout_color_mt( "cout" );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddErrLogger( void )
{
  auto logger = spdlog::stderr_logger_mt( "err" );
  logger->set_level( spdlog::level::debug );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddErrLoggerWithColors( void )
{
  auto logger = spdlog::stderr_color_mt( "err" );
  logger->set_level( spdlog::level::debug );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddFileLogger( const std::string& fileName )
{
  auto logger = spdlog::basic_logger_mt( fileName, fileName );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddDailyFileLogger( const std::string& fileName, const int& hour, const int& minute )
{
  auto logger = spdlog::daily_logger_mt( fileName, fileName, hour, minute );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize, const size_t& maxNumberOfFiles )
{
  auto logger = spdlog::rotating_logger_mt( fileName, fileName, maxFileSize, maxNumberOfFiles );
  this->m_Loggers.push_back( logger );
}

void
VariadicLogger
::RemoveLogger( const std::string& name )
{
  spdlog::drop( name );
}

} // namespace
