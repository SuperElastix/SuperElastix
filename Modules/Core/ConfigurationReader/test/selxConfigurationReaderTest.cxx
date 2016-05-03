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
	ConfigurationReader reader;
  ConfigurationReader::BlueprintPointerType blueprint;

  EXPECT_NO_THROW(blueprint = reader.FromXML("parameterfile2.xml"));
  blueprint->WriteBlueprint("parameterfile2.dot");
	
}

