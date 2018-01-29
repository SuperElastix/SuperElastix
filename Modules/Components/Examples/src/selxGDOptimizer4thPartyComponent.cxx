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

#include "selxGDOptimizer4thPartyComponent.h"

namespace selx
{
GDOptimizer4thPartyComponent::GDOptimizer4thPartyComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  this->theImplementation = new Example4thParty::GDOptimizer4thParty();
  this->MetricObject      = new Metric4thPartyWrapper();
}


GDOptimizer4thPartyComponent::~GDOptimizer4thPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}


int
GDOptimizer4thPartyComponent::Accept( MetricValueInterface::Pointer component )
{
  this->MetricObject->SetMetricValueComponent( component );
  return 0;
}


int
GDOptimizer4thPartyComponent::Update()
{
  this->theImplementation->SetMetric( this->MetricObject );
  return this->theImplementation->DoOptimization(); // 4th party specific call
}


bool
GDOptimizer4thPartyComponent
::MeetsCriterion( const CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  if( criterion.first == "NameOfClass" )
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != "GDOptimizer4rdPartyComponent" )  // e.g. "GradientDescent", "SupportsSparseSamples
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
      }
    }
  }
  return meetsCriteria;
}
} //end namespace selx
