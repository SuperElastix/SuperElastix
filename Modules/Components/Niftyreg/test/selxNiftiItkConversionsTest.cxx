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

#include "selxDataManager.h"

#include "selxItkToNiftiImage.h"
#include "selxNiftiToItkImage.h"
#include "_reg_ReadWriteImage.h"

#include "itkImageFileReader.h"
#include "itkTestingComparisonImageFilter.h"

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
    dataManager = DataManager::New();
  }

  virtual void TearDown() override
  {
  }
  DataManager::Pointer dataManager;
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
  itkImage->Allocate(); // initializePixels = true has no effect for vector images

  itk::Vector<float, 3>* itkImageData = itkImage->GetBufferPointer();
  itkImageData[0][0] = 0.0f;

  auto niftiImage = selx::ItkToNiftiImage<itkImageType, float>::Convert(itkImage);
  // data is copied, the refcount of the itkimage is not increased
  ASSERT_EQ(initialRefCount, itkImage->GetReferenceCount());

  auto niftiImageData = static_cast<float*>(niftiImage->data);

  ASSERT_EQ(0, niftiImageData[0]);
  ASSERT_EQ(0, itkImageData[0][0]);

  niftiImageData[0] = 1.0f;
  itkImageData[0][0] = 2.0f;

  ASSERT_EQ(1.0f, niftiImageData[0]);
  ASSERT_EQ(2.0f, itkImageData[0][0]);
}

TEST_F(NiftiItkConversionsTest, NiftiToItkImage)
{
  // ordinary itk images have the same data layout as nifti images, so data will be shared
  std::string fileName("r16slice.nii.gz");
  using itkImageType = itk::Image<float, 2>;
  itkImageType::Pointer itkImage;
  { // nifti_image only exists in this scope
    std::shared_ptr< nifti_image > niftiImage(reg_io_ReadImageFile(this->dataManager->GetInputFile(fileName).c_str()), nifti_image_free);
    itkImage = selx::NiftiToItkImage<itkImageType, float>::Convert(niftiImage); 
  } // itkImage should live

  // access all data in itkImage by a compareFilter to see if it persisted.
  auto compareFilter = itk::Testing::ComparisonImageFilter<itkImageType, itkImageType>::New();
  compareFilter->SetTestInput(itkImage);

  auto itkReader = itk::ImageFileReader<itkImageType>::New();
  itkReader->SetFileName(fileName);
  compareFilter->SetValidInput(itkReader->GetOutput());

  ASSERT_EQ(0.0f, compareFilter->GetTotalDifference());
}

}
