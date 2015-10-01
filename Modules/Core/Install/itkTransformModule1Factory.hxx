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
#include "itkTransformModule1Factory.h"
#include "itkTransformModule1.h"
#include "itkVersion.h"

namespace itk
{

TransformModule1Factory::TransformModule1Factory()
{
  this->RegisterOverride( "itkModuleBase",
                          "itkTransformModule1",
                          "Transform Module 1",
                          1,
                          CreateObjectFunction< TransformModule1 >::New() ); //float 
}


const char *
TransformModule1Factory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char *
TransformModule1Factory::GetDescription() const
{
  return "Test module 1 for transforms";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.

/*static bool TransformModule1FactoryHasBeenRegistered;

void TransformModule1FactoryRegister__Private(void)
{
  if( ! TransformModule1FactoryHasBeenRegistered )
    {
    TransformModule1FactoryHasBeenRegistered = true;
    TransformModule1Factory::RegisterOneFactory();
    }
}
*/
} // end namespace itk
