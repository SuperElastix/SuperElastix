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
  this->m_Logger = boost::log::sources::severity_logger< SeverityLevel >();

  // Add LineID, TimeStamp, ProcessID and ThreadID
  boost::log::add_common_attributes();

  // TODO: Initialize parameters with values from blueprint
  boost::log::add_file_log(
    boost::log::keywords::file_name = "SuperElastix_%Y-%m-%d_%H-%M-%S.%N.log",
    boost::log::keywords::rotation_size = 1024 * 1024 * 1024, // 1GB
    boost::log::keywords::format = "[%LineID% %TimeStamp% %ComponentName% %SeverityLevel%]: %Message%"
  );

  boost::log::add_console_log(
    std::cout, 
    boost::log::keywords::format = "[%TimeStamp% %ComponentName% %SeverityLevel%]: %Message%"
  );

}

void
Logger::LoggerImpl
::Log( SeverityLevel severityLevel, const std::string message )
{
  boost::log::record record = this->m_Logger.open_record( boost::log::keywords::severity = severityLevel );
  if( record )
  {
    boost::log::record_ostream strm( record );
    strm << message;
    strm.flush();
    this->m_Logger.push_record( boost::move( record ) );
  }
}

}

#endif // Logger_cxx
