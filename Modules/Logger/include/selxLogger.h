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

#ifndef selxLoggerController_h
#define selxLoggerController_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"

namespace selx
{

enum class LogLevel
{
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4,
  CRITICAL = 5,
  OFF = 6
};

class LoggerImpl;

class Logger : public itk::DataObject
{
public:

  /** Standard ITK typedefs. */
  typedef Logger                          Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::DataObject );

  Logger();

  typedef std::unique_ptr< LoggerImpl > LoggerImplPointer;

  void SetLogLevel( const LogLevel& level );
  void SetPattern( const std::string& pattern );

  void SetSyncMode();
  void SetAsyncMode();
  void SetAsyncQueueBlockOnOverflow(void);
  void SetAsyncQueueDiscardOnOverflow(void);
  void SetAsyncQueueSize( const size_t& queueSize );
  void AsyncQueueFlush();

  // TODO: AddStreamWithColors, AddRotatingFileBySize, AddRotatingFileByTime
  void AddStream( std::ostream& stream, const std::string& identifier, const bool& force_flush = false );
  void RemoveStream( const std::string& identifier );
  void RemoveAllStreams( void );

  void Log( const LogLevel& level, const std::string& message, const std::string& name = "General" );

  LoggerImpl& GetLogger( void );


private:

  LoggerImplPointer m_LoggerImpl;

};
} // namespace

#endif // selxLoggerController_h
