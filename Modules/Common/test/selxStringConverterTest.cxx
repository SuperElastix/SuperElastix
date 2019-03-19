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

#include <gtest/gtest.h>

#include "selxStringConverter.h"

#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

using selx::StringConverter;

namespace
{
  template <typename T>
  void ExpectConvertsValue(const T inputValue)
  {
    std::ostringstream outputStringStream;

    outputStringStream << std::setprecision(17) << inputValue;
    const auto str = outputStringStream.str();
    const T convertedValue = StringConverter{ str };

    EXPECT_EQ(convertedValue, inputValue);
  }


  template <typename T>
  void ExpectConvertsToZero(const std::string& str = "0")
  {
    const T convertedValue = StringConverter{ str };
    EXPECT_EQ(convertedValue, 0);
  }


  template <typename T>
  void ExpectConvertsToOne(const std::string& str = "1")
  {
    const T convertedValue = StringConverter{ str };
    EXPECT_EQ(convertedValue, 1);
  }


  template <typename T>
  void ExpectConvertsNumericLimits()
  {
    ExpectConvertsValue(std::numeric_limits<T>::lowest());
    ExpectConvertsValue(std::numeric_limits<T>::min());
    ExpectConvertsValue(std::numeric_limits<T>::max());
  }

} // End of namespace


GTEST_TEST(StringConverter, ConvertsToZero)
{
  ExpectConvertsToZero<short>();
  ExpectConvertsToZero<unsigned short>();
  ExpectConvertsToZero<int>();
  ExpectConvertsToZero<unsigned>();
  ExpectConvertsToZero<long>();
  ExpectConvertsToZero<unsigned long>();
  ExpectConvertsToOne<long long>();
  ExpectConvertsToZero<unsigned long long>();
  ExpectConvertsToZero<float>();
  ExpectConvertsToZero<double>();
  ExpectConvertsToZero<float>("0.0");
  ExpectConvertsToZero<double>("0.0");
}


GTEST_TEST(StringConverter, ConvertsToOne)
{
  ExpectConvertsToOne<short>();
  ExpectConvertsToOne<unsigned short>();
  ExpectConvertsToOne<int>();
  ExpectConvertsToOne<unsigned>();
  ExpectConvertsToOne<long>();
  ExpectConvertsToOne<unsigned long>();
  ExpectConvertsToOne<long long>();
  ExpectConvertsToOne<unsigned long long>();
  ExpectConvertsToOne<float>();
  ExpectConvertsToOne<double>();
  ExpectConvertsToOne<float>("1.0");
  ExpectConvertsToOne<double>("1.0");
}


GTEST_TEST(StringConverter, ConvertsNumericLimits)
{
  ExpectConvertsNumericLimits<short>();
  ExpectConvertsNumericLimits<unsigned short>();
  ExpectConvertsNumericLimits<int>();
  ExpectConvertsNumericLimits<unsigned int>();
  ExpectConvertsNumericLimits<long>();
  ExpectConvertsNumericLimits<unsigned long>();
  ExpectConvertsNumericLimits<long long>();
  ExpectConvertsNumericLimits<unsigned long long>();
  ExpectConvertsNumericLimits<float>();
  ExpectConvertsNumericLimits<double>();
}


GTEST_TEST(StringConverter, ConvertsBool)
{
  const bool expectedTrue = StringConverter{ "True" };
  EXPECT_TRUE(expectedTrue);

  const bool expectedFalse = StringConverter{ "False" };
  EXPECT_FALSE(expectedFalse);
}


GTEST_TEST(StringConverter, ThrowsOnEmptyString)
{
  EXPECT_THROW(int{ StringConverter{ "" } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ " " } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "\n" } }, std::exception);
}


GTEST_TEST(StringConverter, ThrowsOnTrailingCharacters)
{
  EXPECT_THROW(int{ StringConverter{ "1 " } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "1\t" } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "1\r\n" } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "1A" } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "127 " } }, std::exception);
  EXPECT_THROW(int{ StringConverter{ "127A" } }, std::exception);
}
