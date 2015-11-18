#include "elxElastixComponent.h"
#include "gtest/gtest.h"

using namespace elx;

TEST( ElastixComponent, Instantiation )
{
  typedef itk::Image< float, 2u > ImageType;
  typedef ElastixComponent< ImageType > ElastixComponentType;

  ElastixComponentType::Pointer elastix = ElastixComponentType::New();
  
}