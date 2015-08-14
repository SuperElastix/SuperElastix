#ifndef __DataManager_cxx
#define __DataManager_cxx

#include "elxDataManager.h"

std::string
DataManager
::GetInput( const std::string filename ) const 
{
  const std::string path = this->GetInputDirectory() + this->GetPathSeparator() + filename;
  return path;
}

std::string
DataManager
::GetOutput( const std::string filename ) const
{
  const std::string path = this->GetOutputDirectory() + this->GetPathSeparator() + filename;
  return path;
}

std::string
DataManager
::GetBaseline( const std::string filename ) const
{
  const std::string path = this->GetBaselineDirectory() + this->GetPathSeparator() + filename;
  return path;
}

#endif // __elxTestData_cxx