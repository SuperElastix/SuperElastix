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

#include "selxGDOptimizer3rdPartyComponent.h"

namespace selx
{
GDOptimizer3rdPartyComponent::GDOptimizer3rdPartyComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  this->theImplementation = new Example3rdParty::GDOptimizer3rdParty();
  this->MetricObject      = new Metric3rdPartyWrapper();
}


GDOptimizer3rdPartyComponent::~GDOptimizer3rdPartyComponent()
{
  delete this->theImplementation;
  delete this->MetricObject;
}


int
GDOptimizer3rdPartyComponent::Accept( MetricValueInterface::Pointer component )
{
  this->MetricObject->SetMetricValueComponent( component );
  return 0;
}


int
GDOptimizer3rdPartyComponent::Accept( MetricDerivativeInterface::Pointer component )
{
  this->MetricObject->SetMetricDerivativeComponent( component );
  return 0;
}


int
GDOptimizer3rdPartyComponent::Update()
{
  this->theImplementation->SetMetric( this->MetricObject );
  return this->theImplementation->Optimize(); // 3rd party specific call
}


bool
GDOptimizer3rdPartyComponent
::MeetsCriterion( const CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  if( criterion.first == "NameOfClass" )
  {
    meetsCriteria = true;
    for( auto const & criterionValue : criterion.second ) // auto&& preferred?
    {
      if( criterionValue != "GDOptimizer3rdPartyComponent" )  // e.g. "GradientDescent", "SupportsSparseSamples
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
