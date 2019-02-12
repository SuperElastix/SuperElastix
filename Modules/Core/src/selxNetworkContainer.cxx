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

#include "selxNetworkContainer.h"
#include "selxKeys.h"
#include "selxSuperElastixComponent.h"

namespace selx
{
NetworkContainer::NetworkContainer( ComponentContainerType components, UpdateOrderType beforeUpdateOrder, UpdateOrderType updateOrder, OutputObjectsMapType outputObjectsMap ) :
  m_ComponentContainer( components ),
  m_BeforeUpdateOrder( beforeUpdateOrder ),
  m_UpdateOrder( updateOrder),
  m_OutputObjectsMap( outputObjectsMap )
{
}

void
NetworkContainer::BeforeUpdate()
{
  // For components that need to setup internal state before executing the network.
  // The interface is executed in the right pipeline order.
  for( const auto& updateInterface : this->m_BeforeUpdateOrder )
  {
    updateInterface->BeforeUpdate();
  }
}

void
NetworkContainer::Update()
{
  // For components that need to do active work when the network is executed.
  // // The interface is executed in the right pipeline order.
  for( const auto& updateInterface : this->m_UpdateOrder )
  {
    updateInterface->Update();
  }
}


NetworkContainer::OutputObjectsMapType
NetworkContainer::GetOutputObjectsMap()
{
  return this->m_OutputObjectsMap;
}
} //end namespace selx
