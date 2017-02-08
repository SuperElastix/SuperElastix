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

#ifndef LoggerImpl_h
#define LoggerImpl_h

#include "selxLogger.h"
#include "boost/log/expressions/keyword.hpp"
#include "boost/log/sources/severity_channel_logger.hpp"

namespace selx {

BOOST_LOG_ATTRIBUTE_KEYWORD( severity_filter, "Severity", SeverityType );
BOOST_LOG_ATTRIBUTE_KEYWORD( channel_filter, "Channel", Logger::ChannelType );

class Logger::LoggerImpl {
public:
  typedef boost::log::sources::severity_channel_logger< SeverityType, ChannelType > LoggerType;

  LoggerImpl();
  ~LoggerImpl();

  void AddConsole( FormatType format );

  void AddFile( Logger::FileNameType fileName, Logger::FormatType format );

  // void AddFile( Logger::FileNameType fileName, Logger::ChannelType channel, ðLogger::FormatType format );

  void Log( SeverityType severity, Logger::MessageType message );
  void Log( Logger::ChannelType channel, SeverityType severity, Logger::MessageType message );

  LoggerType m_Logger;

};

}

#endif // LoggerImpl_h