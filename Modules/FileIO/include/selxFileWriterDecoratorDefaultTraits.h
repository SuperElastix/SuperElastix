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

#include "itkImageFileWriter.h"
#include "itkMeshFileWriter.h"
#include "itkTransformFileWriter.h"

#include "selxStaticErrorMessageRevealT.h"

namespace selx
{
/**
 * This traits class defines DerivedInputDataType which equals
 * either the ImageType of an ImageFileWriter or the MeshType
 * of a MeshFileWriter.
 * For custom Datatype Writers you can supply your own traits
 * class to the FileWriterDecorator.
 */
template< typename T >
struct FileWriterDecoratorDefaultTraits
{
  static_assert(StaticErrorMessageRevealT< T >::False, "Please Implement FileWriterDecoratorDefaultTraits<TWriter> for this TWriter");
};

template< typename T1 >
struct FileWriterDecoratorDefaultTraits< itk::ImageFileWriter< T1 >>
{
  typedef typename itk::ImageFileWriter< T1 >::InputImageType DerivedInputDataType;
};

template< typename T1 >
struct FileWriterDecoratorDefaultTraits< itk::MeshFileWriter< T1 >>
{
  typedef typename itk::MeshFileWriter< T1 >::InputMeshType DerivedInputDataType;
};

template< typename T1 >
struct FileWriterDecoratorDefaultTraits< itk::TransformFileWriterTemplate< T1 >>
{
  typedef typename itk::TransformFileWriterTemplate< T1 >::TransformType DerivedInputDataType;
};

}
