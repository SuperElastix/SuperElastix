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

#ifndef Providing_hxx
#define Providing_hxx

namespace selx
{
template< typename FirstInterface, typename ... RestInterfaces >
unsigned int
Providing< FirstInterface, RestInterfaces ... >::CountMeetsCriteria( const ComponentBase::InterfaceCriteriaType interfaceCriteria )
{
  return Count< FirstInterface, RestInterfaces ... >::MeetsCriteria( interfaceCriteria );
}
} //end namespace selx
#endif // Providing_hxx
