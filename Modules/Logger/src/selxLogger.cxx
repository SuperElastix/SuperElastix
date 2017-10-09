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
#include "selxLoggerImpl.h"

namespace selx {

Logger
::Logger() {
  this->m_LoggerImpl = LoggerImplPointer(new LoggerImpl());
}

LoggerImpl &
Logger
::GetLogger(void) {
  if (this->m_LoggerImpl)
  {
    return *this->m_LoggerImpl;
  }
  else
  {
    itkExceptionMacro("LoggerImpl not set.");
  }
}

void
Logger
::SetLogLevel( const LogLevel& level )
{
  this->m_LoggerImpl->SetLogLevel( level );
}

void
Logger
::SetFormat( const std::string& format )
{
  this->m_LoggerImpl->SetFormat( format );
}

void
Logger
::SetSyncMode()
{
  this->m_LoggerImpl->SetSyncMode();
}

void
Logger
::SetAsyncMode()
{
  this->m_LoggerImpl->SetAsyncMode();
}

void
Logger
::SetAsyncBlockOnOverflow( void )
{
  this->m_LoggerImpl->SetAsyncBlockOnOverflow();
}

void
Logger
::SetAsyncDiscardOnOverflow( void )
{
  this->m_LoggerImpl->SetAsyncDiscardOnOverflow();
}

void
Logger
::SetAsyncQueueSize( const size_t& queueSize )
{
  this->m_LoggerImpl->SetAsyncQueueSize( queueSize );
}

void
Logger
::Trace( const std::string& message )
{
  this->m_LoggerImpl->Trace( message );
}

void
Logger
::Debug( const std::string& message )
{
  this->m_LoggerImpl->Debug( message );
}

void
Logger
::Info( const std::string& message )
{
  this->m_LoggerImpl->Info( message );
}

void
Logger
::Warning( const std::string& message )
{
  this->m_LoggerImpl->Warning( message );
}

void
Logger
::Error( const std::string& message )
{
  this->m_LoggerImpl->Error( message );
}

void
Logger
::Critical( const std::string& message )
{
  this->m_LoggerImpl->Critical( message );
}

void
Logger
::AddOutLogger( void )
{
  this->m_LoggerImpl->AddOutLogger();
}

void
Logger
::AddOutLoggerWithColors( void )
{
  this->m_LoggerImpl->AddOutLoggerWithColors();
}

void
Logger
::AddErrLogger( void )
{
  this->m_LoggerImpl->AddErrLogger();
}

void
Logger
::AddErrLoggerWithColors( void )
{
  this->m_LoggerImpl->AddErrLoggerWithColors();
}

void
Logger
::AddFileLogger( const std::string& fileName )
{
  this->m_LoggerImpl->AddFileLogger( fileName );
}

void
Logger
::AddDailyFileLogger( const std::string& fileName, const int& hours, const int& minutes )
{
  this->m_LoggerImpl->AddDailyFileLogger( fileName, hours, minutes );
}

void
Logger
::AddRotatingFileLogger( const std::string& fileName, const size_t& maxFileSize, const size_t& maxNumberOfFiles )
{
  this->m_LoggerImpl->AddRotatingFileLogger( fileName, maxFileSize, maxNumberOfFiles );
}

void
Logger
::RemoveLogger( const std::string& name )
{
  this->m_LoggerImpl->RemoveLogger( name );
}



} // namespace