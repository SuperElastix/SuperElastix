#ifndef __DataManager_cxx
#define __DataManager_cxx

#include <itkSimpleDataObjectDecorator.h>
#include "elxDataManager.h"

const std::string
DataManager
::GetInputFile( const std::string filename ) const 
{
  const std::string path = this->GetInputDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

const std::string
DataManager
::GetOutputFile( const std::string filename ) const
{
  const std::string path = this->GetOutputDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

const std::string
DataManager
::GetBaselineFile( const std::string filename ) const
{
  const std::string path = this->GetBaselineDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

#endif // __DataManager_cxx