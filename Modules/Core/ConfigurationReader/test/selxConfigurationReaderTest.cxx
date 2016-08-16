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

#include "selxConfigurationReader.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

using namespace selx;

class ConfigurationReaderTest : public ::testing::Test
{
public:

  typedef DataManager DataManagerType;

  DataManagerType::Pointer dataManager;

  virtual void SetUp()
  {
    this->dataManager = DataManagerType::New();
  }
};

TEST_F( ConfigurationReaderTest, ReadXML )
{
  ConfigurationReader::BlueprintPointerType blueprint;

  EXPECT_NO_THROW( blueprint = ConfigurationReader::FromXML( this->dataManager->GetConfigurationFile( "itkv4_SVF_ANTsCC.xml" ) ) );
  blueprint->WriteBlueprint( this->dataManager->GetOutputFile( "configurationReaderTest_itkv4_SVF_ANTsCC.xml.dot" ) );
}

TEST_F( ConfigurationReaderTest, ReadJson )
{
  ConfigurationReader::BlueprintPointerType blueprint;

  EXPECT_NO_THROW( blueprint = ConfigurationReader::FromJson( this->dataManager->GetConfigurationFile( "itkv4_SVF_ANTsCC.json" ) ) );
  blueprint->WriteBlueprint( this->dataManager->GetOutputFile( "configurationReaderTest_itkv4_SVF_ANTsCC.json.dot" ) );
}
