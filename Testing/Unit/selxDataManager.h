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

#ifndef __DataManager_h
#define __DataManager_h

#include <string>
#include <itkSimpleDataObjectDecorator.h>

#include "selxMacro.h"
#include "itkObjectFactory.h"
#include "itkLightObject.h"
#include "selxDataDirectories.h"

class DataManager : public itk::LightObject
{
public:

  selxNewMacro( DataManager, itk::LightObject );

  DataManager()
  {
    this->m_InputDirectory = SUPERELASTIX_UNITTEST_INPUT_DATA_DIR;
    this->m_OutputDirectory = SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR;
    this->m_BaselineDirectory = SUPERELASTIX_UNITTEST_BASELINE_DATA_DIR;
    this->m_ConfigurationDirectory = SUPERELASTIX_UNITTEST_CONFIGURATION_DATA_DIR;
  }

  std::string GetInputDirectory( void ) const { return this->m_InputDirectory; };
  std::string GetOutputDirectory( void ) const { return this->m_OutputDirectory; };
  std::string GetBaselineDirectory( void ) const { return this->m_BaselineDirectory; };
  std::string GetConfigurationDirectory(void) const { return this->m_ConfigurationDirectory; };

  const std::string GetInputFile( const std::string filename ) const;
  const std::string GetOutputFile( const std::string filename ) const;
  const std::string GetBaselineFile( const std::string filename ) const;
  const std::string GetConfigurationFile(const std::string filename) const;
  

  std::string GetFolderSeparator() const
  {
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
  } 

  std::string GetPathSeparator() const
  {
#ifdef _WIN32
    return ";";
#else
    return ":";
#endif
  }

private:

  std::string m_InputDirectory;
  std::string m_BaselineDirectory;
  std::string m_OutputDirectory;
  std::string m_ConfigurationDirectory;
};

#endif // __DataManager_h