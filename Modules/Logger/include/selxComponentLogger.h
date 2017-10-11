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

#ifndef selxComponentLogger_h
#define selxComponentLogger_h

namespace selx
{

class ComponentLogger
{
public:

  template <typename... Args> void Trace( const char* fmt, const Args&... args );
  template <typename... Args> void Debug( const char* fmt, const Args&... args);
  template <typename... Args> void Info( const char* fmt, const Args&... args );
  template <typename... Args> void Warning( const char* fmt,  const Args&... args );
  template <typename... Args> void Error( const char* fmt, const Args&... args );
  template <typename... Args> void Critical( const char* fmt, const Args&... args );

};
} // namespace

#endif // selxLogger_h
