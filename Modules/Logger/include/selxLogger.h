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

// There already is an sitkLogger preprocessor definition in ITK
#ifndef selxLogger_h
#define selxLogger_h

#include <string>
#include <memory>

#include "itkDataObject.h"
#include "itkObjectFactory.h"

namespace selx
{

enum class SeverityType {
  SELX_TRACE,
  SELX_DEBUG,
  SELX_INFO,
  SELX_WARNING,
  SELX_ERROR,
  SELX_FATAL
};

class LoggerImpl;

class Logger : public itk::DataObject
{
public:

  /** Standard ITK typedefs. */
  typedef Logger                Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::DataObject );

  /* m_Logger is initialized in the default constructor */
  Logger();
  LoggerImpl & GetLogger( void );

  /** The actual logger is a pimpled member variable */
  typedef std::shared_ptr< LoggerImpl > LoggerPointer;

  typedef const std::string ChannelType;
  typedef const std::string FormatType;
  typedef const std::string MessageType;

  void AddConsole( FormatType format = "[%LineID% %TimeStamp%]: %Message%" );

  // void AddFile( FileNameType fileName = "SuperElastix_%Y-%m-%d_%H-%M-%S.%N.log",
  //               FormatType format = "[%LineID% %TimeStamp% %ComponentName% %Channel% %SeverityLevel%]: %Message%" );
  // void AddFile( FileNameType fileName = "SuperElastix_%Y-%m-%d_%H-%M-%S.%N.log",
  //               LoggerImpl::ChannelType channel = "SuperElastix",
  //               FormatType format = "[%LineID% %TimeStamp% %ComponentName% %Channel% %SeverityLevel%]: %Message%" );

  virtual void Log( SeverityType severity, MessageType message );

  // void Log( ChannelType channel, SeverityType severity, MessageType message );



private:

  LoggerPointer m_Logger;
};
} // namespace

#endif // selxLogger_h
