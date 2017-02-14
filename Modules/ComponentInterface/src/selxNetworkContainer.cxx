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
  NetworkContainer::NetworkContainer(ComponentContainerType components, OutputObjectsMapType outputObjectsMap) : m_ComponentContainer(components), m_OutputObjectsMap(outputObjectsMap)
  {
  }


  void
    NetworkContainer::Execute()
  {
    /** Scans all Components to find those with RegistrationControllerStart capability and call them */
    for (auto const & component : this->m_ComponentContainer)
    {
      if (component->CountProvidingInterfaces({ { keys::NameOfInterface, keys::RegistrationControllerStartInterface } }) == 1)
      {
        std::shared_ptr<RegistrationControllerStartInterface> providingInterface = std::dynamic_pointer_cast<RegistrationControllerStartInterface>(component);
        if (!providingInterface)  // is actually a double-check for sanity: based on criterion cast should always be successful
        {
          throw std::runtime_error("dynamic_cast<RegistrationControllerStartInterface*> fails, but based on component criterion it shouldn't");
        }
        providingInterface->RegistrationControllerStart();
      }
    }
  }

  NetworkContainer::OutputObjectsMapType NetworkContainer::GetOutputObjectsMap()
  {
    return this->m_OutputObjectsMap;
  }

} //end namespace selx
