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

//Component group NiftyregItkHybrid
#include "selxNiftyregItkMultiStageComponent.h"
#include "selxItkToNiftiImageHybridSourceComponent.h"

namespace selx
{
using ModuleNiftyregItkHybridComponents = selx::TypeList<
    ItkToNiftiImageHybridSourceComponent<2, float>,
    ItkToNiftiImageHybridSourceComponent<2, double>,
    ItkToNiftiImageHybridSourceComponent<3, float>,
    ItkToNiftiImageHybridSourceComponent<3, double>,
		ItkToNiftiImageHybridSourceComponent<2, unsigned char>,
	  ItkToNiftiImageHybridSourceComponent<3, unsigned char>,
    NiftyregItkMultiStageComponent<float, 2>,
	  NiftyregItkMultiStageComponent<double, 2>,
	  NiftyregItkMultiStageComponent<double, 3>,
	  NiftyregItkMultiStageComponent<float, 3>
  >;
}
