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

#ifndef selxVariadicLogger_hxx
#define selxVariadicLogger_hxx

#include "selxVariadicLogger.h"

namespace selx
{

template <typename... Args>
void
VariadicLogger
::Trace( const char* fmt, const Args& ...args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->trace( fmt, args... );
  }
}

template <typename... Args>
void
VariadicLogger
::Debug( const char* fmt, const Args& ...args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->debug( fmt, args... );
  }
}

template <typename... Args>
void
VariadicLogger
::Info( const char* fmt, const Args&... args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->info( fmt, args... );
  }
}

template <typename... Args>
void
VariadicLogger
::Warning( const char* fmt, const Args&... args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->warn( fmt, args... );
  }
}

template <typename... Args>
void
VariadicLogger
::Error( const char* fmt, const Args&... args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->error( fmt, args... );
  }
}

template <typename... Args>
void
VariadicLogger
::Critical( const char* fmt, const Args&... args )
{
  for( const auto& logger : this->m_Loggers )
  {
    logger->critical( fmt, args... );
  }
}

} // namespace

#endif // selxVariadicLogger_h