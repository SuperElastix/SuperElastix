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

#include "selxLogger.h"

#include "gtest/gtest.h"

using namespace selx;

TEST( LoggerTest, Initialization )
{
  Logger::Pointer logger = Logger::New();
}

TEST( LoggerTest, Console )
{
  Logger::Pointer logger = Logger::New();
  logger->AddOutLoggerWithColors();

  // TODO: Explicitly test output
  logger->Trace( "Console TRACE message" );
  logger->Debug( "Console DEBUG message" );
  logger->Info( "Console INFO message" );
  logger->Warning( "Console WARNING message" );
  logger->Error( "Console ERROR message" );
  logger->Critical( "Console FATAL message" );

  logger->SetLogLevel( LogLevel::TRACE );
  logger->Trace( "Console TRACE message" );
  logger->Debug( "Console DEBUG message" );
}

// This will fail with "logger with name 'cout' already exists" if logger above is not properly deallocatd
TEST( LoggerTest, MemoryManagement )
{
  Logger::Pointer logger = Logger::New();
  logger->AddOutLoggerWithColors();
}

TEST( LoggerTest, Singleton )
{
  Logger::Pointer logger = Logger::New();
  logger->AddOutLogger();
  EXPECT_THROW( logger->AddOutLogger(), std::exception );

  logger->AddFileLogger( "fileLogger.log" );
  EXPECT_THROW( logger->AddFileLogger( "fileLogger.log" ), std::exception );

  logger->AddDailyFileLogger( "dailyFileLogger.log" );
  EXPECT_THROW( logger->AddDailyFileLogger( "dailyFileLogger.log" ), std::exception );

  logger->AddRotatingFileLogger( "rotatingFileLogger.log" );
  EXPECT_THROW( logger->AddRotatingFileLogger( "dailyFileLogger.log" ), std::exception );
}

TEST( LoggerTest, MultiSink )
{
  Logger::Pointer logger = Logger::New();
  logger->AddOutLogger();
  logger->AddErrLogger();
  logger->AddFileLogger( "fileLogger.log" );
  logger->AddDailyFileLogger( "dailyFileLogger.log" );
  logger->AddRotatingFileLogger( "rotatingFileLogger.log" );
  logger->Info( "Multi-sink INFO message" );
}

