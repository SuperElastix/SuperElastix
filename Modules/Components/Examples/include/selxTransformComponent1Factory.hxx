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
#include "itkTransformComponent1Factory.h"
#include "itkTransformComponent1.h"
#include "itkVersion.h"

namespace selx
{
TransformComponent1Factory::TransformComponent1Factory()
{
  this->RegisterOverride( "ComponentBase",
    "itkTransformComponent1",
    "Transform Component 1",
    1,
    itk::CreateObjectFunction< TransformComponent1 >::New() );                      //float
}


const char *
TransformComponent1Factory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}


const char *
TransformComponent1Factory::GetDescription() const
{
  return "Test module 1 for transforms";
}


// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.

/*static bool TransformComponent1FactoryHasBeenRegistered;

void TransformComponent1FactoryRegister__Private(void)
{
  if( ! TransformComponent1FactoryHasBeenRegistered )
    {
    TransformComponent1FactoryHasBeenRegistered = true;
    TransformComponent1Factory::RegisterOneFactory();
    }
}
*/
} // end namespace selx
