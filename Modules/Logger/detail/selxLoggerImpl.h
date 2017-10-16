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
#include "spdlog/sinks/ostream_sink.h"

namespace selx
{

class LoggerImpl
{

public:

  typedef spdlog::async_overflow_policy AsyncQueueOverflowPolicyType;

  LoggerImpl();

  void SetLogLevel( const LogLevel& level );
  void SetPattern( const std::string& pattern );

  void SetSyncMode();
  void SetAsyncMode();
  void SetAsyncQueueBlockOnOverflow(void);
  void SetAsyncQueueDiscardOnOverflow(void);
  void SetAsyncQueueSize( const size_t& queueSize );
  void AsyncQueueFlush();

  void AddStream( std::ostream& stream, const std::string& identifier, const bool& forceFlush = false );
  void RemoveStream( const std::string& identifier );
  void RemoveAllStreams( void );

  void Log( const LogLevel& level, const std::string& message, const std::string& name = "NoName" );

  template <typename spdlog::level::level_enum level, typename ... Args>
  void
  Log( const std::string& fmt, const Args& ... args )
  {
  	for( const auto& logger : this->m_Loggers )
  	{
    	logger->log( level, fmt, args ... );
  	}
  }

private:

  typedef std::shared_ptr< spdlog::logger > LoggerType;
  typedef std::vector< LoggerType > LoggerVectorType;

  spdlog::level::level_enum ToSpdLogLevel( const LogLevel& level );

  size_t m_AsyncQueueSize;
  AsyncQueueOverflowPolicyType m_AsyncQueueOverflowPolicy;
	LoggerVectorType m_Loggers;

};

} // namespace

#endif // selxLoggerImpl_h
