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

#include "gtest/gtest.h"

#include "selxItkNiftiUser.h"
#include "selxNiftyregNiftiUser.h"

namespace selx
{
class NiftiNameManglingTest : public ::testing::Test
{
public:

  const NiftiFunctionPtrs itkNiftiFunctionPtrs;
  const NiftiFunctionPtrs niftyregNiftiFunctionPtrs;

  NiftiNameManglingTest()
    :
    itkNiftiFunctionPtrs( ItkNiftiUser::GetNiftiFunctionPtrs() ),
    niftyregNiftiFunctionPtrs( NiftyregNiftiUser::GetNiftiFunctionPtrs() )
  {
  }


  virtual void SetUp() override
  {
  }


  virtual void TearDown() override
  {
  }
};

// The name mangling should ensure that common functions from different nifti library instantiations
// (either fromn ITK or Niftyreg) can de distinguished by their function address.
TEST_F( NiftiNameManglingTest, AddressesOfFunctionsFromDifferentNiftiLibsAreDifferent )
{
  // Sanity checks to see if the function pointers are properly initialized.
  ASSERT_NE( nullptr, itkNiftiFunctionPtrs.disp_lib_version_FunctionPtr );
  ASSERT_NE( nullptr, niftyregNiftiFunctionPtrs.disp_lib_version_FunctionPtr );

  // The main test: the function addresses should be different.
  ASSERT_NE( itkNiftiFunctionPtrs.disp_lib_version_FunctionPtr,
    niftyregNiftiFunctionPtrs.disp_lib_version_FunctionPtr );
}

// Function calls from either ITK or Niftyreg should usually have the same effect.
TEST_F( NiftiNameManglingTest, FunctionCallsFromDifferentNiftiLibsHaveSameEffect )
{
  // Sanity checks of the test itself, to prevent dereferencing a null pointer:
  ASSERT_NE( nullptr, itkNiftiFunctionPtrs.get_filesize_FunctionPtr );
  ASSERT_NE( nullptr, itkNiftiFunctionPtrs.swap_2bytes_FunctionPtr );
  ASSERT_NE( nullptr, niftyregNiftiFunctionPtrs.swap_2bytes_FunctionPtr );
  ASSERT_NE( nullptr, niftyregNiftiFunctionPtrs.get_filesize_FunctionPtr );

  ASSERT_EQ( itkNiftiFunctionPtrs.get_filesize_FunctionPtr( nullptr ),
    niftyregNiftiFunctionPtrs.get_filesize_FunctionPtr( nullptr ) );
  ASSERT_EQ( itkNiftiFunctionPtrs.get_filesize_FunctionPtr( "" ),
    niftyregNiftiFunctionPtrs.get_filesize_FunctionPtr( "" ) );

  int itkData = 1;
  itkNiftiFunctionPtrs.swap_2bytes_FunctionPtr( sizeof( int ), &itkData );

  int niftyregData = 1;
  niftyregNiftiFunctionPtrs.swap_2bytes_FunctionPtr( sizeof( int ), &niftyregData );

  ASSERT_EQ( itkData, niftyregData );
}
}
