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
    EXPECT_NO_THROW(reader.loadxml("parameterfile2.xml"));
	
}

