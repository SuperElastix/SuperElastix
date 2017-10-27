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

#include "selxItkToNiftiImage.h"
#include "selxNiftiToItkImage.h"

namespace selx
{
class NiftiItkConversionsTest : public ::testing::Test
{
public:

  NiftiItkConversionsTest()
  {
  }


  virtual void SetUp() override
  {
  }


  virtual void TearDown() override
  {
  }
};

TEST_F(NiftiItkConversionsTest, ItkToNiftiImageSharedDataRemoveNifti)
{
  // ordinary itk images have the same data layout as nifti images, so data will be shared
  using itkImageType = itk::Image<float, 3>;
  auto itkImage = itkImageType::New();
  auto initialRefCount = itkImage->GetReferenceCount();

  itkImage->SetRegions({ 16, 16, 16 });
  itkImage->Allocate(true);
  {
    auto niftiImage = selx::ItkToNiftiImage<itkImageType, float>::Convert(itkImage);
    // data is shared, the refcount of the itkimage is increased
    ASSERT_EQ(initialRefCount + 1, itkImage->GetReferenceCount());
  }
  // destruction of niftiImage should decrease refcount again
  ASSERT_EQ(initialRefCount, itkImage->GetReferenceCount());
}

TEST_F(NiftiItkConversionsTest, ItkToNiftiImageSharedDataRemoveItk)
{
  using itkImageType = itk::Image<float, 3>;
  std::shared_ptr<nifti_image> niftiImage;
  itkImageType* rawitkpointer;
  {
    // ordinary itk images have the same data layout as nifti images, so data will be shared
    
    auto itkImage = itkImageType::New();
    rawitkpointer = itkImage.GetPointer();
    auto initialRefCount = itkImage->GetReferenceCount();

    itkImage->SetRegions({ 16, 16, 16 });
    itkImage->Allocate(true);
    niftiImage = selx::ItkToNiftiImage<itkImageType, float>::Convert(itkImage);
  }
  // destruction of niftiImage should decrease refcount again
  ASSERT_EQ(1, rawitkpointer->GetReferenceCount());
  auto floatdata = static_cast<float*>(niftiImage->data);
  for (unsigned int index = 0; index < 16 * 16 * 16; ++index)
  {
    ASSERT_EQ(0, floatdata[index]);
    // Test if memory still exists by writing to it.
    ASSERT_NO_THROW(floatdata[index] = 42.0f);
  }
}

TEST_F(NiftiItkConversionsTest, ItkToNiftiImageCopiedData)
{
  // itk vector images have a different data layout than nifti images, so data will be copied
  using itkImageType = itk::Image<itk::Vector<float,3>, 3>;
  auto itkImage = itkImageType::New();
  auto initialRefCount = itkImage->GetReferenceCount();

  itkImage->SetRegions({ 16, 16, 16 });
  itkImage->Allocate(true);
  {
    auto niftiImage = selx::ItkToNiftiImage<itkImageType, float>::Convert(itkImage);
    // data is copied, the refcount of the itkimage is not increased
    ASSERT_EQ(initialRefCount, itkImage->GetReferenceCount());
  }
}

}
