#include "itkImage.h"
#include "itkImageFileReader.h"
#include "elxDataManager.h"
#include "gtest/gtest.h"

class itkRegistration : public ::testing::Test {
public:
  virtual void SetUp()
  {
    DataManager dataManager;
    typedef itk::ImageFileReader< ImageType > ReaderType;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( dataManager.GetInput( "BrainProtonDensitySliceR10X13Y17.png" ) );
    inputImage = reader->GetOutput();

    reader->SetFileName( dataManager.GetBaseline( "BrainProtonDensitySlice.png" ) );
    baselineImage = reader->GetOutput();
  }

  typedef itk::Image< unsigned int, 2 > ImageType;
  ImageType::Pointer inputImage;
  ImageType::Pointer baselineImage;
};

TEST_F( itkRegistration, Affine2D )
{
  ASSERT_TRUE( true );
}
