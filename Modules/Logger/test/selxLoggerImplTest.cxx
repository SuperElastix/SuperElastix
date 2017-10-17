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

TEST( LoggerImplTest, Initialization )
{
  LoggerImpl logger = LoggerImpl();
}

TEST( LoggerImplTest, Console )
{
  LoggerImpl logger = LoggerImpl();
  logger.AddStream( "cout", std::cout );

  // TODO: Explicitly test output
  logger.Log( LogLevel::TRC, "Console TRACE message" );
  logger.Log( LogLevel::DBG, "Console DEBUG message" );
  logger.Log( LogLevel::INF, "Console INFO message" );
  logger.Log( LogLevel::WRN, "Console WARNING message" );
  logger.Log( LogLevel::ERR ,"Console ERROR message" );
  logger.Log( LogLevel::CRT, "Console FATAL message" );

  logger.SetLogLevel( LogLevel::TRC );
  logger.Log( LogLevel::TRC, "Console TRACE message" );
  logger.Log( LogLevel::DBG, "Console DEBUG message" );

  logger.Log( LogLevel::INF, "Console INFO message with pattern {1:d}", "LoggerImplTest", 42 );
}


 TEST( LoggerImplTest, MemoryManagement )
 {
   // This will fail with "logger with name 'cout' already exists" if logger above is not properly destroyed
   LoggerImpl logger = LoggerImpl();
   logger.AddStream( "cout", std::cout );
 }
