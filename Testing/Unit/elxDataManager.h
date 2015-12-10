#ifndef __DataManager_h
#define __DataManager_h

#include <string>
#include <itkSimpleDataObjectDecorator.h>

#include "elxMacro.h"
#include "itkObjectFactory.h"
#include "itkLightObject.h"
#include "elxDataDirectories.h"

class DataManager : public itk::LightObject
{
public:

  elxNewMacro( DataManager, itk::LightObject );

  DataManager()
  {
    this->m_InputDirectory = SUPERELASTIX_UNITTEST_INPUT_DATA_DIR;
    this->m_OutputDirectory = SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR;
    this->m_BaselineDirectory = SUPERELASTIX_UNITTEST_BASELINE_DATA_DIR;
  }

  std::string GetInputDirectory( void ) const { return this->m_InputDirectory; };
  std::string GetOutputDirectory( void ) const { return this->m_OutputDirectory; };
  std::string GetBaselineDirectory( void ) const { return this->m_BaselineDirectory; };

  const std::string GetInputFile( const std::string filename ) const;
  const std::string GetOutputFile( const std::string filename ) const;
  const std::string GetBaselineFile( const std::string filename ) const;

  std::string GetFolderSeparator() const
  {
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
  } 

  std::string GetPathSeparator () const
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

};

#endif // __DataManager_h