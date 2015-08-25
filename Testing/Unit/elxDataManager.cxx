#ifndef __DataManager_cxx
#define __DataManager_cxx

#include <itkSimpleDataObjectDecorator.h>
#include "elxDataManager.h"

const std::string
DataManager
::GetInputFullPath( const std::string filename ) const 
{
  const std::string path = this->GetInputDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

const std::string
DataManager
::GetOutputFullPath( const std::string filename ) const
{
  const std::string path = this->GetOutputDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

const std::string
DataManager
::GetBaselineFullPath( const std::string filename ) const
{
  const std::string path = this->GetBaselineDirectory() + this->GetFolderSeparator() + filename;
  return path;
}

#endif // __elxTestData_cxx