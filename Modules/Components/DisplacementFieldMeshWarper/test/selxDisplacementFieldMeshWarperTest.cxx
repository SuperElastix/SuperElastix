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
#include "selxItkVectorImageSourceComponent.h"
#include "selxItkVectorImageSinkComponent.h"
#include "selxItkMeshSourceComponent.h"
#include "selxItkMeshSinkComponent.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "selxDisplacementFieldMeshWarperComponent.h"

#include "gtest/gtest.h"
#include "selxDataManager.h"

namespace selx {

class DisplacementFieldMeshWarperComponentTest : public ::testing::Test {
public:

  typedef itk::VectorImage< float, 2 >            VectorImageType;
  typedef typename VectorImageType::Pointer       VectorImagePointer;
  typedef itk::ImageFileReader< VectorImageType > VectorImageReaderType;
  typedef typename VectorImageReaderType::Pointer VectorImageReaderPointer;
  typedef itk::ImageFileWriter< VectorImageType > VectorImageWriterType;
  typedef typename VectorImageWriterType::Pointer VectorImageWriterPointer;

  typedef itk::Mesh< float, 2 >                   MeshType;
  typedef typename MeshType::Pointer              MeshPointer;
  typedef itk::MeshFileReader< MeshType >         MeshReaderType;
  typedef typename MeshReaderType::Pointer        MeshReaderPointer;
  typedef itk::MeshFileWriter< MeshType >         MeshWriterType;
  typedef typename MeshWriterType::Pointer        MeshWriterPointer;

  typedef Blueprint::Pointer BlueprintPointer;

  typedef TypeList<
    ItkVectorImageSourceComponent< 2, float >,
    ItkVectorImageSinkComponent< 2, float >,
    ItkMeshSourceComponent< 2, float >,
    ItkMeshSinkComponent< 2, float >,
    ItkDisplacementFieldMeshWarperComponent< 2, float, float > > TestComponents;

  typedef SuperElastixFilterCustomComponents< TestComponents > SuperElastixFilterType;
  typedef typename SuperElastixFilterType::Pointer SuperElastixFilterPointer;
};

TEST_F( DisplacementFieldMeshWarperComponentTest, SinkAndSource )
{
  BlueprintPointer blueprint = Blueprint::New();
  using ParameterMapType = Blueprint::ParameterMapType;

  ParameterMapType vectorImageSourceParameters;
  vectorImageSourceParameters[ "NameOfClass" ]    = { "ItkVectorImageSourceComponent" };
  vectorImageSourceParameters[ "Dimensionality" ] = { "2" };
  blueprint->SetComponent( "DisplacementFieldSource", vectorImageSourceParameters );

  SuperElastixFilterPointer superElastixFilter = SuperElastixFilterType::New();
  superElastixFilter->SetBlueprint(blueprint);
}

} // namespace selx