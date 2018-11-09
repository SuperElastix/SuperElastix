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

#include <iterator>

#include "selxLogger.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/fmt/bundled/ostream.h"

namespace selx
{

class LoggerImpl
{

public:

  typedef spdlog::async_overflow_policy AsyncQueueOverflowPolicyType;

  LoggerImpl();
  ~LoggerImpl();

  void SetLogLevel( const LogLevel& level );
  void SetPattern( const std::string& pattern );

  void SetSyncMode();
  void SetAsyncMode();
  void SetAsyncQueueBlockOnOverflow(void);
  void SetAsyncQueueDiscardOnOverflow(void);
  void SetAsyncQueueSize( const size_t& queueSize );
  void AsyncQueueFlush();

  void AddStream( const std::string& identifier, std::ostream& stream, const bool& forceFlush = false );
  void RemoveStream( const std::string& identifier );
  void RemoveAllStreams( void );

  void Log( const LogLevel& level, const std::string& message );

  template < typename ... Args >
  void
  Log( const LogLevel& level, const std::string& fmt, const Args& ... args )
  {
  	for( const auto& identifierAndLogger : this->m_Loggers )
  	{
      identifierAndLogger.second->log( this->ToSpdLogLevel( level ), fmt.c_str(), args ... );
  	}
  }

  // Stream std:vector to string
  // TODO: Use std::copy_n to print [n1, n2, ... , n-1, n] if vector is long
  template < typename T >
  std::string ToString( const std::vector< T >& v ) const {
    std::ostringstream out;
    if( !v.empty() ) {
      if( v.size() > 1 ) out << '[';
      std::copy( v.begin(), v.end(), std::ostream_iterator< T >( out, ", " ) );
      RemoveTrailingCharsFromOutputStringStream(out, 2);
      if( v.size() > 1 ) out << "]";
    }
    return out.str();
  }

  // Stream std::map< T, T > to string
  template < typename T >
  std::string ToString( const std::map< T, T >& m ) const {
    std::ostringstream out;
    if( !m.empty() ) {
      out << "{";
      for( const auto& item : m )
      {
        out << item.first << ": " << item.second << ", ";
      }
      RemoveTrailingCharsFromOutputStringStream(out, 2);
      out << "}";
    }
    return out.str();
  }

  // Stream std::map< T, std::vector< T > > to string
  template < typename T >
  std::string ToString( const std::map< T, std::vector< T > >& m ) const {
    std::ostringstream out;
    if( !m.empty() ) {
      out << "{";
      for( const auto& item : m )
      {
        out << item.first << ": " << this << item.second << ", ";
      }
      RemoveTrailingCharsFromOutputStringStream(out, 2);
      out << "}";
    }
    return out.str();
  }

private:
  static void RemoveTrailingCharsFromOutputStringStream(
    std::ostringstream& outputStringStream,
    unsigned numberOfCharsToRemove)
  {
    auto str = outputStringStream.str();
    const auto numberOfCharsOfString = str.size();
    if (numberOfCharsOfString >= numberOfCharsToRemove)
    {
      str.erase(numberOfCharsOfString - numberOfCharsToRemove);
      outputStringStream.str(str);
    }
  }

  // Spdlog configuration
  spdlog::level::level_enum ToSpdLogLevel( const LogLevel& level );
  size_t m_AsyncQueueSize;
  AsyncQueueOverflowPolicyType m_AsyncQueueOverflowPolicy;

  // Logger container
  typedef std::shared_ptr< spdlog::logger > LoggerType;
  typedef std::map< std::string, LoggerType > LoggerVectorType;
  LoggerVectorType m_Loggers;

};

} // namespace

#endif // selxLoggerImpl_h
