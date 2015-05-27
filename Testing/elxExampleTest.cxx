#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "gtest/gtest.h"

TEST( GoogleTest, GoogleTest ) {
  /**
   * This example demonstrates the GoogleTest framework.
   *
   * When the test is built, the ExternalData CMake module 
   * scans the Testing/Data directory for content links on  
   * the form "[filename].[ext].md5" (text files with hashes).
   * The content links are replaced with files from the file
   * repository and checked for hash consistensy.
   */

  ASSERT_TRUE( true );
}