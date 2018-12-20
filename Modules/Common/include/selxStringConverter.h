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

namespace selx
{
  class StringConverter
  {
    class Exception: public std::exception
    {
      friend class StringConverter;
      
      const char* m_What;
      
      explicit Exception(const char* const arg)
      :
      m_What{arg}
      {
      }
      
    public:
      const char* what() const noexcept override
      {
        return m_What;
      }
    };

  private:
  
    std::string m_String;
    
  public:

    explicit StringConverter(std::string arg)
    :
    m_String{std::move(arg)}
    {
    }
  
    operator bool() const
    {
      // TODO Consider supporting the strings from https://docs.python.org/3/distutils/apiref.html?highlight=distutils.util#distutils.util.strtobool

      if( m_String == "True" )
      {
        return true;
      }
      
      if( m_String == "False" )
      {
        return false;
      }
      throw Exception{ __FUNCTION__ " failed!" };
    }
    

    operator double() const
    {
      std::size_t index = 0;
      const auto value = std::stod(m_String, &index);

      if (index != m_String.size())
      {
        throw Exception{ __FUNCTION__ " failed!" };
      }
      return value;
    }      

    operator float() const
    {
      std::size_t index = 0;
      const auto value = std::stof(m_String, &index);

      if (index != m_String.size())
      {
        throw Exception{ __FUNCTION__ " failed!" };
      }
      return value;
    }

    operator int() const
    {
      std::size_t index = 0;
      const auto value = std::stoi(m_String, &index);

      if (index != m_String.size())
      {
        throw Exception{ __FUNCTION__ " failed!" };
      }
      return value;
    }

    operator unsigned long() const
    {
      std::size_t index = 0;
      const auto value = std::stoul(m_String, &index);

      if (index != m_String.size())
      {
        throw Exception{ __FUNCTION__ " failed!" };
      }
      return value;
    }


    template <typename T>
    T To()
    {
      return { *this };
    }
  };

}

#endif
