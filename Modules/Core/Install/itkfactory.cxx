/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkDisplacementFieldTransform.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkCompositeTransform.h"
#include "itkVector.h"
#include "itkPoint.h"

#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkMatrixOffsetTransformBase.h"

//Floris: The module factory is based on the transformIO factory. 
// By default it creates 2 dummy modules: itkTransformModule1 and itkMetricModule1
#include "itkModuleFactoryBase.h"
#include "itkModuleFactory.h"


int main(int argc, char *argv[])
{
  std::string fileName;
  if (argc == 1) // No arguments were provided
  {
    fileName = "test.tfm";
  }
  else
  {
    fileName = argv[1];
  }

  typedef itk::MatrixOffsetTransformBase< double, 3, 3 > MatrixOffsetTransformType;
  itk::TransformFactory<MatrixOffsetTransformType>::RegisterTransform();

#if (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 5) || ITK_VERSION_MAJOR > 4
  itk::TransformFileReaderTemplate<float>::Pointer reader =
    itk::TransformFileReaderTemplate<float>::New();
#else
  itk::TransformFileReader::Pointer reader = itk::TransformFileReader::New();
#endif
  reader->SetFileName(fileName);
  reader->Update();

  std::cout << *(reader->GetTransformList()->begin()) << std::endl;

  return EXIT_SUCCESS;
}