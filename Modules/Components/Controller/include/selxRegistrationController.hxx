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

#include "selxRegistrationController.h"

namespace selx
{
template< bool dummy >
RegistrationControllerComponent< dummy >::RegistrationControllerComponent()
{
}


template< bool dummy >
RegistrationControllerComponent< dummy >::~RegistrationControllerComponent()
{
}


template< bool dummy >
int
RegistrationControllerComponent< dummy >::Set( RunRegistrationInterface * other )
{
  this->m_RunRegistrationInterfaces.insert( other );
  return 0;
}


template< bool dummy >
int
RegistrationControllerComponent< dummy >::Set( ReconnectTransformInterface * other )
{
  this->m_ReconnectTransformInterfaces.insert( other );
  return 0;
}


template< bool dummy >
int
RegistrationControllerComponent< dummy >::Set( AfterRegistrationInterface * other )
{
  this->m_AfterRegistrationInterfaces.insert( other );
  return 0;
}


template< bool dummy >
void
RegistrationControllerComponent< dummy >::RegistrationControllerStart()
{
  for( auto && runRegistrationInterface : this->m_RunRegistrationInterfaces )
  {
    runRegistrationInterface->RunRegistration();
  }
  for( auto && reconnectTransformInterface : this->m_ReconnectTransformInterfaces )
  {
    reconnectTransformInterface->ReconnectTransform();
  }
  //for (auto && afterRegistrationInterface : this->m_AfterRegistrationInterfaces)
  //{
  //  afterRegistrationInterface->AfterRegistration();
  //}
}


template< bool dummy >
bool
RegistrationControllerComponent< dummy >::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria(false);
  bool meetsCriteria(false);
  auto status = CheckTemplateProperties(this->TemplateProperties(), criterion);
  if (status == CriterionStatus::Satisfied)
  {
    return true;
  }
  else if (status == CriterionStatus::Failed)
  {
    return false;
  } // else: CriterionStatus::Unknown
  return meetsCriteria;
}
} //end namespace selx
