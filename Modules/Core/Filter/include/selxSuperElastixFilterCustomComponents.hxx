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

#ifndef selxSuperElastixFilterCustomComponents_hxx
#define selxSuperElastixFilterCustomComponents_hxx

#include "selxSuperElastixFilterCustomComponents.h"
#include "selxRegisterComponentFactoriesByTypeList.h"
#include "selxNetworkBuilder.h"


namespace selx
{
/**
 * ********************* Constructor *********************
 */
  template<typename ComponentTypeList>
  SuperElastixFilterCustomComponents<ComponentTypeList>
    ::SuperElastixFilterCustomComponents(void) : SuperElastixFilter(true)
{
  //RegisterFactoriesByTypeList< ComponentTypeList >::Register();
  m_NetworkBuilder = std::make_unique<NetworkBuilder<ComponentTypeList>>();
} // end Constructor

} // namespace elx

#endif // selxSuperElastixFilterCustomComponents_hxx
