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

#ifndef Count_hxx
#define Count_hxx

namespace selx
{

template< typename FirstInterface, typename ... RestInterfaces >
unsigned int
Count< FirstInterface, RestInterfaces ... >::MeetsCriteria( const ComponentBase::InterfaceCriteriaType interfaceCriteria )
{
  auto interfaceProperies = Properties< FirstInterface >::Get();
  for( const auto keyAndValue : interfaceCriteria )
  {
    auto && key   = keyAndValue.first;
    auto && value = keyAndValue.second;
    if( interfaceProperies.count( key ) != 1 || interfaceProperies[ key ].compare( value ) != 0 )
    {
      // as soon as any of the criteria fails we break the loop and test the RestInterfaces
      return Count< RestInterfaces ... >::MeetsCriteria( interfaceCriteria );
    }
  }
  // if all criteria are met for this Interface we add 1 to the count and continue with the RestInterfaces
  return 1 + Count< RestInterfaces ... >::MeetsCriteria( interfaceCriteria );
}

} //end namespace selx
#endif // Count_hxx