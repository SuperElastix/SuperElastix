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

#include "selxSSDMetric4thPartyComponent.h"
namespace selx
{
SSDMetric4thPartyComponent::SSDMetric4thPartyComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  this->theImplementation = new Example4thParty::SSDMetric4thParty();
}


SSDMetric4thPartyComponent::~SSDMetric4thPartyComponent()
{
  delete this->theImplementation;
}


int
SSDMetric4thPartyComponent::GetValue()
{
  return this->theImplementation->GetCost(); // translate method name
}


bool
SSDMetric4thPartyComponent
::MeetsCriterion( const CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  if( criterion.first == "NameOfClass" )
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != "SSDMetric4thPartyComponent" )  // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "ComponentProperty" )
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != "SomeProperty" )  // e.g. "GradientDescent", "SupportsSparseSamples
      {
        meetsCriteria = false;
        return false;
      }
    }
  }
  return meetsCriteria;
}
} //end namespace selx
