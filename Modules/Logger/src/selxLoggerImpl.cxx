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
#include "spdlog/details/registry.h"

namespace selx
{

LoggerImpl
::LoggerImpl() : m_Loggers(), m_AsyncQueueSize( 262144 ), m_AsyncQueueOverflowPolicy( spdlog::async_overflow_policy::block_retry )
{
  this->SetSyncMode();
  this->SetPattern( "[%Y-%m-%d %H:%M:%S] [thread %t] [%l] [{0}] %v" );
}

spdlog::level::level_enum
LoggerImpl
::ToSpdLogLevel( const LogLevel& level ) {
  switch (level) {
    case LogLevel::TRACE:
      return spdlog::level::level_enum::trace;
      break;
    case LogLevel::DEBUG:
      return spdlog::level::level_enum::debug;
      break;
    case LogLevel::INFO:
      return spdlog::level::level_enum::info;
      break;
    case LogLevel::WARNING:
      return spdlog::level::level_enum::warn;
      break;
    case LogLevel::ERROR:
      return spdlog::level::level_enum::err;
      break;
    case LogLevel::CRITICAL:
      return spdlog::level::level_enum::critical;
      break;
    case LogLevel::OFF:
      return spdlog::level::level_enum::off;
      break;
    default:
      itkGenericExceptionMacro( "Invalid log level." );
  }
}

void
LoggerImpl
::SetLogLevel( const LogLevel& level ) {
  spdlog::level::level_enum spdloglevel = this->ToSpdLogLevel( level );
  spdlog::set_level( spdloglevel );
  for( const auto& logger : this->m_Loggers )
  {
    logger->set_level( spdloglevel );
  }
}

void
LoggerImpl
::SetPattern( const std::string& pattern )
{
  spdlog::set_pattern( pattern );
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
  spdlog::set_async_mode(this->m_AsyncQueueSize, this->m_AsyncQueueOverflowPolicy);
}

void
LoggerImpl
::SetAsyncQueueBlockOnOverflow(void)
{
  this->m_AsyncQueueOverflowPolicy = AsyncQueueOverflowPolicyType::block_retry;
  this->SetAsyncMode();
}

void
LoggerImpl
::SetAsyncQueueDiscardOnOverflow(void)
{
  this->m_AsyncQueueOverflowPolicy = AsyncQueueOverflowPolicyType::discard_log_msg;
  this->SetAsyncMode();
}

void
LoggerImpl
::SetAsyncQueueSize( const size_t& queueSize )
{
  this->m_AsyncQueueSize = queueSize;
  this->SetAsyncMode();
}

void
LoggerImpl
::AddStream( std::ostream& stream, const std::string& name, const bool& forceFlush )
{
  auto sink = std::make_shared< spdlog::sinks::ostream_sink< std::mutex > >(stream, forceFlush);
  auto logger = spdlog::details::registry::instance().create(name, { sink } );
  this->m_Loggers.push_back( logger );
}

void
LoggerImpl
::RemoveStream( const std::string& name )
{
  spdlog::drop( name );
}

void
LoggerImpl
::RemoveAllStreams( void )
{
  spdlog::drop_all();
}

void
LoggerImpl
::Log( const LogLevel& level, const std::string& message, const std::string& name )
{
  spdlog::level::level_enum spdLogLevel = this->ToSpdLogLevel( level );
  for( const auto& logger : this->m_Loggers )
  {
    logger->log( spdLogLevel, message.c_str(), name );
  }
}


} // namespace
