#include "selxConfigurationReader.h"

#include "gtest/gtest.h"

using namespace selx;

class ConfigurationReaderTest : public ::testing::Test {
public:
  virtual void SetUp() {
  }
};

TEST_F( ConfigurationReaderTest, ReadXML )
{
  ConfigurationReader::BlueprintPointerType blueprint;

  EXPECT_NO_THROW(blueprint = ConfigurationReader::FromXML("configurationReaderTest.xml"));
  blueprint->WriteBlueprint("configurationReaderTest.xml.dot");
	
}

TEST_F(ConfigurationReaderTest, ReadJson)
{
  ConfigurationReader::BlueprintPointerType blueprint;

  EXPECT_NO_THROW(blueprint = ConfigurationReader::FromJson("configurationReaderTest.json"));
  blueprint->WriteBlueprint("configurationReaderTest.json.dot");

}
