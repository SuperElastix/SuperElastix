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
#include "itkModuleFactory.h"
#include "itkVersion.h"

#include "itkTransformModule1.h"
#include "itkMetricModule1.h"

namespace itk
{
ModuleFactoryBase *ModuleFactoryBase:: m_Factory = ITK_NULLPTR;

namespace ModuleFactoryBasePrivate
{
bool DefaultModulesRegistered = false;
}

ModuleFactoryBase::ModuleFactoryBase()
{}

ModuleFactoryBase::~ModuleFactoryBase()
{}

void ModuleFactoryBase::RegisterDefaultModules()
{
  //
  // make sure that the the factory instance has
  // been created. All normal paths to this method
  // already do this but this makes certain sure it's done
  (void)ModuleFactoryBase::GetFactory();

  if ( !ModuleFactoryBasePrivate::DefaultModulesRegistered )
    {
    //
    // double instances (in alphabetical order)
    //
    ModuleFactory< TransformModule1< double, 2 > >::RegisterModule ();
	  ModuleFactory< MetricModule1< double, 2 > >::RegisterModule ();
 
    }
  ModuleFactoryBasePrivate::DefaultModulesRegistered = true;
}

const char *
ModuleFactoryBase::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char *
ModuleFactoryBase::GetDescription() const
{
  return "Module FactoryBase";
}
} // end namespace itk
