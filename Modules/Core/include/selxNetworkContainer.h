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

#ifndef selxNetworkContainer_h
#define selxNetworkContainer_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"

#include "itkDataObject.h"

#include <map>
#include <vector>
#include <memory>

namespace selx
{
class NetworkContainer
{
  // a (light weight) ComponentContainer with 1 Execute button. All other data such as graphs and selectors can be deleted.

public:

  using ComponentContainerType = std::vector< std::shared_ptr< ComponentBase >>;
  using UpdateOrderType = std::vector<std::shared_ptr< UpdateInterface >>;
  using OutputObjectsMapType   = std::map< std::string, itk::DataObject::Pointer >;

  NetworkContainer( ComponentContainerType components, UpdateOrderType beforeUpdateOrder, UpdateOrderType updateOrder, OutputObjectsMapType outputObjectsMap );
  ~NetworkContainer() {}

  /** Allow components to setup internal state before network is updated */
  void BeforeUpdate();

  /** Run the (registration) algorithm */
  void Update();

  /** Get the Sinking output objects */
  OutputObjectsMapType GetOutputObjectsMap();

private:

  const ComponentContainerType m_ComponentContainer;
  const UpdateOrderType m_BeforeUpdateOrder;
  const UpdateOrderType m_UpdateOrder;
  const OutputObjectsMapType   m_OutputObjectsMap;
};
} // end namespace selx
#endif // selxNetworkContainer_h
