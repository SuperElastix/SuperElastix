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

#include "selxTypeList.h"
#include "selxModuleController.h"
#include "selxModuleElastix.h"
#include "selxModuleExamples.h"
#include "selxModuleItkImageRegistrationMethodv4.h"
//#include "selxModuleItkSmoothingRecursiveGaussianImageFilter.h"
//#include "selxModuleItkSyNImageRegistrationMethod.h"
#include "selxModuleNiftyreg.h"
#include "selxModuleSinksAndSources.h"

namespace selx
{
using CompiledLibraryComponents = list_append<
  ModuleControllerComponents,
	ModuleElastixComponents, 
  //ModuleExamplesComponents,
  ModuleItkImageRegistrationMethodv4Components,
  //ModuleItkSmoothingRecursiveGaussianImageFilterComponents,
  //ModuleModuleItkSyNImageRegistrationMethod,
  ModuleNiftyregComponents,
  ModuleSinksAndSourcesComponents
    >;
}
