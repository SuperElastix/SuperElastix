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

#include "gtest/gtest.h"

using namespace selx;
using namespace spdlog::level

TEST( LoggerTest, Initialization )
{
  Logger::Pointer logger = Logger::New();
}

TEST( VariadicLoggerTest, Console )
{
  Logger::Pointer logger = Logger::New();
  logger->AddStream( std::cout );

  // TODO: Explicitly test output
  logger->Log( trace, "Console TRACE message" );
  logger->Log( debug, "Console DEBUG message" );
  logger->Log( info, "Console INFO message" );
  logger->Log( warn, "Console WARNING message" );
  logger->Log( error ,"Console ERROR message" );
  logger->Log( critical, "Console FATAL message" );

  logger->SetLogLevel( trace );
  logger->Log( trace, "Console TRACE message" );
  logger->Log( debug, "Console DEBUG message" );

  logger->Log( info, "Console INFO message with pattern {1:d}", "LoggerImplTest", 42 );
}

// // This will fail with "logger with name 'cout' already exists" if logger above is not properly deallocatd
// TEST( VariadicLoggerTest, MemoryManagement )
// {
//   VariadicLogger::Pointer logger = VariadicLogger::New();
//   logger->AddOutLoggerWithColors();
// }

// TEST( VariadicLoggerTest, Singleton )
// {
//   VariadicLogger::Pointer logger = VariadicLogger::New();
//   logger->AddOutLogger();
//   EXPECT_THROW( logger->AddOutLogger(), std::exception );

//   logger->AddFileLogger( "fileLogger.log" );
//   EXPECT_THROW( logger->AddFileLogger( "fileLogger.log" ), std::exception );

//   logger->AddDailyFileLogger( "dailyFileLogger.log" );
//   EXPECT_THROW( logger->AddDailyFileLogger( "dailyFileLogger.log" ), std::exception );

//   logger->AddRotatingFileLogger( "rotatingFileLogger.log" );
//   EXPECT_THROW( logger->AddRotatingFileLogger( "dailyFileLogger.log" ), std::exception );
// }

// TEST( VariadicLoggerTest, MultiSink )
// {
//   VariadicLogger::Pointer logger = VariadicLogger::New();
//   logger->AddOutLogger();
//   logger->AddErrLogger();
//   logger->AddFileLogger( "fileLogger.log" );
//   logger->AddDailyFileLogger( "dailyFileLogger.log" );
//   logger->AddRotatingFileLogger( "rotatingFileLogger.log" );
//   logger->Info( "Multi-sink INFO message" );
// }

