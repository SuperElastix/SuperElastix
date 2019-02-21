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

//Component group Niftyreg
#include "selxItkToNiftiImageSourceComponent.h"
#include "selxNiftiToItkImageSinkComponent.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"
#include "selxItkToNiftiImageComponent.h"


namespace selx
{
using ModuleNiftyregComponents = selx::TypeList<
  Niftyregf3dComponent< float>,
  ItkToNiftiImageSourceComponent< 2, float >,
  ItkToNiftiImageSourceComponent< 3, float >,
  ItkToNiftiImageSourceComponent< 2, unsigned char >,
  ItkToNiftiImageSourceComponent< 3, unsigned char >,
  ItkToNiftiImageComponent< 2, float >,
  ItkToNiftiImageComponent< 3, float >,
  ItkToNiftiImageComponent< 2, unsigned char >,
  ItkToNiftiImageComponent< 3, unsigned char >,
  NiftiToItkImageSinkComponent< 2, float >,
  NiftiToItkImageSinkComponent< 3, float >,
  NiftyregSplineToDisplacementFieldComponent< float>,
  DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
  DisplacementFieldNiftiToItkImageSinkComponent< 3, float>,
  NiftyregAladinComponent< float >
  >;
}
