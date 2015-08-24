#ifndef __DataManager_h
#define __DataManager_h

#include <string>

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
    this->m_InputDirectory = ELASTIX_UNITTEST_INPUT_DATA_DIR;
    this->m_OutputDirectory = ELASTIX_UNITTEST_OUTPUT_DATA_DIR;
    this->m_BaselineDirectory = ELASTIX_UNITTEST_BASELINE_DATA_DIR;
  }

  std::string GetInputDirectory( void ) const { return this->m_InputDirectory; };
  std::string GetOutputDirectory( void ) const { return this->m_OutputDirectory; };
  std::string GetBaselineDirectory( void ) const { return this->m_BaselineDirectory; };

  std::string GetInputFullPath( const std::string filename ) const;
  std::string GetOutputFullPath( const std::string filename ) const;
  std::string GetBaselineFullPath( const std::string filename ) const;

  std::string GetFolderSeparator() const
  {
#ifdef WIN32
    return "\\";
#else
    return "/";
#endif
  } 

  std::string GetPathSeparator () const
  {
#ifdef WIN32
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

#endif // #define __DataManager_h