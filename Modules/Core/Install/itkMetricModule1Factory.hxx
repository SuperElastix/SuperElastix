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
#include "itkMetricModule1Factory.h"
#include "itkMetricModule1.h"
#include "itkVersion.h"

namespace itk
{

MetricModule1Factory::MetricModule1Factory()
{
  this->RegisterOverride( "itkModuleIOBase",
                          "itkMetricModule1",
                          "Metric Module 1",
                          1,
                          CreateObjectFunction< MetricModule1 >::New() ); //float 
}

const char *
MetricModule1Factory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char *
MetricModule1Factory::GetDescription() const
{
  return "Test module 1 for Metrics";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.
/*
static bool MetricModule1FactoryHasBeenRegistered;

void MetricModule1FactoryRegister__Private(void)
{
  if( ! MetricModule1FactoryHasBeenRegistered )
    {
    MetricModule1FactoryHasBeenRegistered = true;
    MetricModule1Factory::RegisterOneFactory();
    }
}
*/
} // end namespace itk
