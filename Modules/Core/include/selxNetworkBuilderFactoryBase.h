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

#ifndef NetworkBuilderFactoryBase_h
#define NetworkBuilderFactoryBase_h

#include "selxLogger.h"

namespace selx
{
class NetworkBuilderFactoryBase
{
public:

  NetworkBuilderFactoryBase() {}

  virtual ~NetworkBuilderFactoryBase() {}

  /** To create a derived NetworkBuilderFactory from a NetworkBuilderFactoryBase pointer without knowing the template arguments of the derived*/
  virtual std::unique_ptr< NetworkBuilderBase > New( LoggerImpl & logger, BlueprintImpl & blueprint ) = 0;

private:
};
} // end namespace selx

#endif // NetworkBuilderBase_h
