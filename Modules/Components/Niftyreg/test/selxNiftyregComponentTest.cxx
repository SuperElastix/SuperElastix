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

#include "_reg_f3d.h"
#include "selxDataManager.h"
#include "gtest/gtest.h"

//using namespace selx;

class NiftyregComponentTest : public ::testing::Test
{
public:
  
  typedef DataManager DataManagerType;
  virtual void SetUp()
  {
  }


  virtual void TearDown()
  {
  }
};

TEST_F( NiftyregComponentTest, Create )
{
	nifti_image *referenceImage=NULL;
	nifti_image *floatingImage=NULL;
	reg_f3d<float>* REG=new reg_f3d<float>(referenceImage->nt,floatingImage->nt);
}
