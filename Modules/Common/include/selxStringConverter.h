#ifndef selxStringConverter_h
#define selxStringConverter_h

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

#include <exception>
#include <string>
#include <sstream>

namespace selx
{
  class StringConverter
  {
  private:
  
    std::string m_String;
    
  public:

    explicit StringConverter(std::string arg)
    :
    m_String{std::move(arg)}
    {
    }

    // Returns true on success, and false on failure.
    static bool Convert(const std::string& str, bool& convertedValue) noexcept
    {
      // TODO Consider supporting the strings from https://docs.python.org/3/distutils/apiref.html?highlight=distutils.util#distutils.util.strtobool
      if (str == "True")
      {
        convertedValue = true;
        return true;
      }
      if (str == "False")
      {
        convertedValue = false;
        return true;
      }
      return false;
    }


    // Returns true on success, and false on failure.
    template <typename T>
    static bool Convert(const std::string& inputString, T& convertedValue) noexcept
    {
      try
      {
        T result{};

        std::istringstream inputStringStream{ inputString };
        
        inputStringStream >> result;

        if (inputStringStream && inputStringStream.eof())
        {
          convertedValue = result;
          return true;
        }
      }
      catch (const std::exception&)
      {
      }
      return false;
    }


    template <typename T>
    static bool IsStringConvertibleToValue(const std::string& str, const T value) noexcept
    {
      T convertedValue;
      return StringConverter::Convert(str, convertedValue) && (convertedValue == value);

    }


    template <typename T>
    operator T() const
    {
      T result;

      if (StringConverter::Convert(m_String, result))
      {
        return result;
      }

      class Exception final : public std::exception
      {
      public:
        const char* what() const noexcept override
        {
          return "Failed to convert!";
        }
      };

      throw Exception{};
    }
  };

}

#endif
