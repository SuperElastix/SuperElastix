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

#include "selxSuperElastixFilterCustomComponents.h"
#include "selxItkImageSinkComponent.h"
#include "selxItkImageSourceComponent.h"

#include "gtest/gtest.h"
#include "selxDataManager.h"

namespace selx {

class DisplacementFieldMeshWarperComponentTest : public ::testing::Test {
public:
  typedef Blueprint::Pointer BlueprintPointer;

  typedef TypeList<
    ItkVectorImageSourceComponent< float, 2 >,
    ItkVectorImageSourceSinkComponent< float, 2 >,
    ItkMeshSourceComponent< float, 2 >,
    ItkMeshSinkComponent< float, 2 >,
    ItkDisplacementFieldMeshWarperComponent< float, float, 2 > > TestComponents;

  typedef itk::VectorImage< float, 2 >            VectorImageType;
  typedef VectorImageType::Pointer                VectorImagePointer;
  typedef itk::ImageFileReader< VectorImageType > VectorImageReaderType;
  typedef VectorImageReaderType::Pointer          VectorImageReaderPointer;
  typedef itk::ImageFileWriter< VectorImageType > VectorImageWriterType;
  typedef VectorImageWriterType::Pointer          VectorImagePointer;

  typedef itk::Mesh< float, 2 >                   MeshType;
  typedef MeshType::Pointer                       MeshPointer;
  typedef itk::MeshFileReader< float, 2 >         MeshReaderType;
  typedef MeshFileReaderType::Pointer             MeshReaderPointer;
  typedef MeshFileWriter< float, 2 >              MeshWriterType;
  typedef MeshWriterType::Pointer                 MeshWriterPointer;
};

TEST_F( DisplacementFieldMeshWarperComponentTest, SinkAndSource )
{

}

} // namespace selx