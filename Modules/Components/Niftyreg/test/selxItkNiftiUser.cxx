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

// Its own header file.
#include "selxItkNiftiUser.h"

// Note: The unit test NiftiNameManglingTest.AddressesOfFunctionsFromDifferentNiftiLibsAreDifferent
// should fail when you uncomment the next line:
// #define itk_nifti_mangle_h

#include "../ITK-4.12/nifti1_io.h"

namespace selx
{
NiftiFunctionPtrs
ItkNiftiUser::GetNiftiFunctionPtrs()
{
  const NiftiFunctionPtrs result =
  {
    &nifti_disp_lib_version,
    &nifti_get_filesize,
    &nifti_swap_2bytes
  };
  return result;
}
}
