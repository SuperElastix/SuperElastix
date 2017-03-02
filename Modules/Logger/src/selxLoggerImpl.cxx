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

#ifndef Logger_cxx
#define Logger_cxx

#include "selxLoggerImpl.h"

#include "boost/log/sources/record_ostream.hpp"
#include "boost/log/utility/setup/file.hpp"
#include "boost/log/utility/setup/console.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"

namespace selx
{
Logger::LoggerImpl
::LoggerImpl()
{
  this->m_Logger = boost::log::sources::severity_channel_logger< SeverityType, ChannelType >();

  // Add LineID, TimeStamp, ProcessID and ThreadID
  boost::log::add_common_attributes();
}


Logger::LoggerImpl
::~LoggerImpl()
{
}


void
Logger::LoggerImpl
::AddConsole( FormatType format )
{
  boost::log::add_console_log(
    std::cout,
    boost::log::keywords::format = format
    );
}


// void
// Logger::LoggerImpl
// ::AddFile( FileNameType fileName, FormatType format )
// {
//   boost::log::add_file_log(
//     boost::log::keywords::file_name = fileName,
//     boost::log::keywords::format = format
//   );
// }


// void
// Logger::LoggerImpl
// ::AddFile( FileNameType fileName, ChannelType channel, FormatType format )
// {
//   boost::log::add_file_log(
//     boost::log::keywords::file_name = fileName,
//     boost::log::keywords::filter = channel_filter == channel,
//     boost::log::keywords::format = format
//   );
// }


void
Logger::LoggerImpl
::Log( SeverityType severity, MessageType message )
{
  boost::log::record record = this->m_Logger.open_record( boost::log::keywords::severity = severity );
  if( record )
  {
    boost::log::record_ostream strm( record );
    strm << message;
    strm.flush();
    this->m_Logger.push_record( boost::move( record ) );
  }
  else
  {
    assert( false );
  }
}


// void
// Logger::LoggerImpl
// ::Log( Logger::ChannelType channel, SeverityType severity, const std::string message )
// {
//   boost::log::record record = this->m_Logger.open_record( ( boost::log::keywords::channel = channel, boost::log::keywords::severity = severity ) );
//   if( record )
//   {
//     boost::log::record_ostream strm( record );
//     strm << message;
//     strm.flush();
//     this->m_Logger.push_record( boost::move( record ) );
//   }
//   else
//   {
//     assert( false );
//   }
// }


}

#endif // Logger_cxx
