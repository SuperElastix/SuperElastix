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
::LoggerImpl() : m_Loggers(), m_AsyncQueueSize( 262144 ), m_AsyncQueueOverflowPolicy( spdlog::async_overflow_policy::block_retry ), m_StreamToString()
{
  this->SetSyncMode();
  this->SetPattern( "[%Y-%m-%d %H:%M:%S.%f] [thread %t] [%l] %v" );
}

LoggerImpl
::~LoggerImpl()
{
  this->RemoveAllStreams();
}

spdlog::level::level_enum
LoggerImpl
::ToSpdLogLevel( const LogLevel& level ) {
  switch (level) {
    case LogLevel::TRC:
      return spdlog::level::level_enum::trace;
    case LogLevel::DBG:
      return spdlog::level::level_enum::debug;
    case LogLevel::INF:
      return spdlog::level::level_enum::info;
    case LogLevel::WRN:
      return spdlog::level::level_enum::warn;
    case LogLevel::ERR:
      return spdlog::level::level_enum::err;
    case LogLevel::CRT:
      return spdlog::level::level_enum::critical;
    case LogLevel::OFF:
      return spdlog::level::level_enum::off;
    default:
      itkGenericExceptionMacro( "Invalid log level." );
  }
}

void
LoggerImpl
::SetLogLevel( const LogLevel& level ) {
  spdlog::level::level_enum spdloglevel = this->ToSpdLogLevel( level );
  spdlog::set_level( spdloglevel );
  for( const auto& item : this->m_Loggers )
  {
    item.second->set_level( spdloglevel );
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
}

void
LoggerImpl
::SetAsyncQueueDiscardOnOverflow(void)
{
  this->m_AsyncQueueOverflowPolicy = AsyncQueueOverflowPolicyType::discard_log_msg;
}

void
LoggerImpl
::SetAsyncQueueSize( const size_t& queueSize )
{
  this->m_AsyncQueueSize = queueSize;
}

void
LoggerImpl
::AsyncQueueFlush( void )
{
  for( const auto& identifierAndLogger : this->m_Loggers )
  {
    identifierAndLogger.second->flush();
  }
}

void
LoggerImpl
::AddStream( const std::string& identifier, std::ostream& stream, const bool& forceFlush )
{
  auto sink = std::make_shared< spdlog::sinks::ostream_sink< std::mutex > >(stream, forceFlush);
  auto logger = spdlog::details::registry::instance().create(identifier, { sink } );
  this->m_Loggers.insert( std::make_pair( identifier, logger ) );
}

void
LoggerImpl
::RemoveStream( const std::string& name )
{
  spdlog::drop( name );
  this->m_Loggers.erase( name );
}

void
LoggerImpl
::RemoveAllStreams( void )
{
  for( const auto& identifierAndLogger : this->m_Loggers )
  {
    spdlog::drop( identifierAndLogger.first );
  }

  this->m_Loggers.clear();
}

void
LoggerImpl
::Log( const LogLevel& level, const std::string& message )
{
  auto spdLogLevel = this->ToSpdLogLevel( level );
  for( const auto& identifierAndLogger : this->m_Loggers )
  {
    identifierAndLogger.second->log( spdLogLevel, message.c_str() );
  }
}

} // namespace
