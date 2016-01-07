/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
//#ifndef ComponentSelector_hxx
//#define ComponentSelector_hxx

#include "ComponentSelector.h"

namespace selx
{

ComponentSelector::ComponentSelector()
  {
    this->m_PossibleComponents.clear();
  }
ComponentSelector::~ComponentSelector()
  {
  }

void ComponentSelector::Initialize()
{
  std::list< itk::LightObject::Pointer >     allobjects =
    itk::ObjectFactoryBase::CreateAllInstance("ComponentBase");

  for (std::list< itk::LightObject::Pointer >::iterator i = allobjects.begin();
    i != allobjects.end(); ++i)
  {
    ComponentBase *io =
      dynamic_cast<ComponentBase *>(i->GetPointer());
    if (io)
    {
      this->m_PossibleComponents.push_back(io);
    }
  }
}
void ComponentSelector::SetCriteria(const CriteriaType &criteria)
{
  this->Initialize();
  this->m_Criteria = criteria;
  this->Modified();

}

void ComponentSelector::AddCriteria(const CriteriaType &criteria)
{
  this->m_Criteria.insert(criteria.begin(), criteria.end());
  this->Modified();
}

// a predicate implemented as a class:
//struct FailsCriteria {
//  bool operator() (const ComponentBasePointer& component) { return !component->MeetsCriteria(this->m_Criteria) }
//};

ComponentSelector::NumberOfComponentsType ComponentSelector::UpdatePossibleComponents()
{
  // Check each possible component if it meets the criteria
  // Using a Lambda function.
  this->m_PossibleComponents.remove_if([&](ComponentBasePointer component){ return !component->MeetsCriteria(this->m_Criteria); });
  return this->m_PossibleComponents.size();
}
ComponentSelector::ComponentBasePointer ComponentSelector::GetComponent()
{
  //TODO check if Modified
  this->UpdatePossibleComponents();

  if (this->m_PossibleComponents.size() == 1)
  {
    return *(this->m_PossibleComponents.begin());
  }
  else
  {
    return ITK_NULLPTR;
  }
}
} // end namespace selx

//#endif
