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

#ifndef selxElastixInterfaces_h
#define selxElastixInterfaces_h

#include "elxElastixFilter.h"
#include "elxElastixFilter.h"

namespace selx
{
template< typename TFixedImage, typename TMovingImage >
class elastixTransformParameterObjectInterface
{
public:

  using Type    = elastixTransformParameterObjectInterface< TFixedImage, TMovingImage >;
  using Pointer = std::shared_ptr< Type >;
  typedef typename elastix::ElastixFilter< TFixedImage, TMovingImage >::ParameterObjectType elastixTransformParameterObject;
  virtual elastixTransformParameterObject * GetTransformParameterObject() = 0;
};

template< class F, class M >
struct Properties< elastixTransformParameterObjectInterface< F, M >>
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "elastixTransformParameterObjectInterface" } }; // TODO map F and M to strings?
  }
};
} // end namespace selx

#endif // #define selxElastixInterfaces_h
