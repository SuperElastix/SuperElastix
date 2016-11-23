/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
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
#include "itkMetricComponent1Factory.h"
#include "itkMetricComponent1.h"
#include "itkVersion.h"

namespace selx
{
MetricComponent1Factory::MetricComponent1Factory()
{
  this->RegisterOverride( "ComponentBase",
    "itkMetricComponent1",
    "Metric Component 1",
    1,
    itk::CreateObjectFunction< MetricComponent1 >::New() );                      //float
}


const char *
MetricComponent1Factory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}


const char *
MetricComponent1Factory::GetDescription() const
{
  return "Test module 1 for Metrics";
}


// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.
/*
static bool MetricComponent1FactoryHasBeenRegistered;

void MetricComponent1FactoryRegister__Private(void)
{
  if( ! MetricComponent1FactoryHasBeenRegistered )
    {
    MetricComponent1FactoryHasBeenRegistered = true;
    MetricComponent1Factory::RegisterOneFactory();
    }
}
*/
} // end namespace selx
