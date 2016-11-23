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
#include "selxComponentFactory.h"
//#include "itkMetricComponent1.h"
#include "itkVersion.h"
//#include "ComponentTraits.h"
#include <typeinfo>
namespace selx
{
template< class ComponentT >
ComponentFactory< ComponentT >::ComponentFactory()
{
  this->RegisterOverride( "ComponentBase",
    typeid( ComponentT ).name(),                     //(&ComponentT)->GetNameOfClass(), //
    ComponentT::GetDescription(),
    1,
    itk::CreateObjectFunction< ComponentT >::New() );
}


template< class ComponentT >
const char *
ComponentFactory< ComponentT >::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}


template< class ComponentT >
const char *
ComponentFactory< ComponentT >::GetDescription() const
{
  return ComponentT::GetDescription(); //TODO should this be the description of the factory instead of the component?
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
