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

#ifndef selxExamplesInterfaces_h
#define selxExamplesInterfaces_h

namespace selx
{
// Define the providing interfaces abstractly
class MetricDerivativeInterface
{
public:

  using Pointer               = std::shared_ptr< MetricDerivativeInterface >;
  virtual int GetDerivative() = 0;
};

class MetricValueInterface
{
public:

  using Pointer          = std::shared_ptr< MetricValueInterface >;
  virtual int GetValue() = 0;
};

class OptimizerUpdateInterface
{
public:

  using Pointer        = std::shared_ptr< OptimizerUpdateInterface >;
  virtual int Update() = 0;
};

class TransformedImageInterface
{
public:

  using Pointer                     = std::shared_ptr< TransformedImageInterface >;
  virtual int GetTransformedImage() = 0;
};

class ConflictinUpdateInterface
{
public:

  using Pointer = std::shared_ptr< ConflictinUpdateInterface >;

  // "error" : member function templates cannot be virtual
  // template <class ConflictinUpdateInterface> virtual int Update() = 0;
  //TODO http://en.cppreference.com/w/cpp/language/member_template

  //TODO solution: http://stackoverflow.com/questions/2004820/inherit-interfaces-which-share-a-method-name
  //TODO better?: http://stackoverflow.com/questions/18398409/c-inherit-from-multiple-base-classes-with-the-same-virtual-function-name
  virtual int Update( ConflictinUpdateInterface::Pointer ) = 0;
};

template< >
struct Properties< MetricValueInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "MetricValueInterface" } };
  }
};
template< >
struct Properties< MetricDerivativeInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "MetricDerivativeInterface" } };
  }
};
template< >
struct Properties< OptimizerUpdateInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "OptimizerUpdateInterface" } };
  }
};

template< >
struct Properties< ConflictinUpdateInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "ConflictinUpdateInterface" } };
  }
};

template< >
struct Properties< TransformedImageInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "TransformedImageInterface" } };
  }
};
} // end namespace selx

#endif // #define selxExamplesInterfaces_h
