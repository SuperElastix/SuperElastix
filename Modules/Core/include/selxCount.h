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

#ifndef Count_h
#define Count_h

namespace selx
{
// helper class for SuperElastixComponent::CountAcceptingInterfaces and SuperElastixComponent::CountProvidingInterfaces to loop over a set of interfaces

template< typename ... Interfaces >
struct Count;

template< >
struct Count< >
{
  static unsigned int MeetsCriteria( const ComponentBase::InterfaceCriteriaType ) { return 0; }
};

template< typename FirstInterface, typename ... RestInterfaces >
struct Count< FirstInterface, RestInterfaces ... >
{
  static unsigned int MeetsCriteria( const ComponentBase::InterfaceCriteriaType );
};
} //end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxCount.hxx"
#endif
#endif // Count_h
