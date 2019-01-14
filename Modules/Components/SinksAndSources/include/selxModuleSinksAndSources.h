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

//Component group SinksAndSources
#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxItkDisplacementFieldSinkComponent.h"
#include "selxItkImageSourceComponent.h"
#include "selxItkImageSinkComponent.h"
#include "selxItkMeshSourceComponent.h"
#include "selxItkMeshSinkComponent.h"

namespace selx
{
using ModuleSinksAndSourcesComponents = selx::TypeList<
  ItkImageSourceComponent< 2, float >,
  ItkImageSourceComponent< 2, short >,
  ItkImageSourceComponent< 2, unsigned char >,
  ItkImageSourceComponent< 3, float >,
  ItkImageSourceComponent< 3, short >,
  ItkImageSourceComponent< 3, unsigned char >,
  ItkImageSinkComponent< 2, float >,
  ItkImageSinkComponent< 2, short >,
  ItkImageSinkComponent< 3, float >,
  ItkImageSinkComponent< 3, short >,
  ItkMeshSourceComponent< 2, float >,
  ItkMeshSourceComponent< 3, float >,
  ItkMeshSinkComponent< 2, float >,
  ItkMeshSinkComponent< 3, float >,
  ItkDisplacementFieldSourceComponent< 2, float >,
  ItkDisplacementFieldSourceComponent< 3, float >,
  ItkDisplacementFieldSinkComponent< 2, float >,
  ItkDisplacementFieldSinkComponent< 3, float >
  >;
}
