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

#ifndef selxDisplacementFieldWarperComponent_h
#define selxDisplacementFieldWarperComponent_h

#include "selxSuperElastixComponent.h"
#include "selxItkObjectInterfaces.h"

#include "itkDisplacementFieldTransform.h"

//
// Accepting< ItkVectorImageInterface< Dimensionality, TPixel >, ItkMeshInterface< Dimensionality, TCoordinateType > >,
//Providing< ItkMeshInterface< Dimensionality, TCoordinateType > >

namespace selx {

template< TPixel, TCoordinateType, int Dimension >
class ItkDisplacementFieldMeshWarperComponent : public
  SuperElastixComponent<
    Accepting< itkVectorImageInterface< TPixel, Dimensionality >, itkMeshInterface< Dimension, TPixel > >,
    Providing< itkMeshInterface< Dimension, TPixel > > >
{
public:
  typedef DisplacementFieldWarperComponent Self;
  typedef SuperElastixComponent<
  Accepting< ItkVectorImageInterface, itkMeshInterface >,
  Providing< ItkMeshInterface > > Superclass;
  typedef std::shared_ptr< Self > Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldMeshWarperComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkDisplacementFieldMeshWarperComponent();

  using DisplacementFieldType = using ItkVectorImageInterface< Dimensionality, TPixel >::Type;
  typedef DisplacementFieldType::Pointer DisplacementFieldPointer;

  using MeshType = ItkVectorImageInterface< Dimensionality, TPixel >::Type;
  typedef MeshType::Pointer MeshPointer;

  typedef itk::DisplacementFieldTransform< TPoint, Dimension > DisplacementFieldTransformType;
  typedef typename DisplacementFieldTransformType::Pointer DisplacementFieldTransformPointer ;

  typedef ResampleImageFilter<DisplacementFieldType, DisplacementFieldType, Dimension> ResampleImageFilterType;
  typedef typename ResampleImageFilterType::Pointer ResampleImageFilterPointer;

  virtual int Accept( DisplacementFieldPointer displacementField );
  virtual int Accept( MeshPointer mesh );

  MeshType * GetWarpedItkMesh();

  static const char * GetDescription() { return "Warp a point set based on a deformation field"; };

protected:

  // return the class name and the template arguments to identify this component.
  static inline const TemplatePropertiesType TemplateProperties()
  {
    return {
      { keys::NameOfClass, "ItkDisplacementFieldMeshWarperComponent" },
      { keys::PixelType, PodString< TPixel >::Get() },
      { keys::InternalCoordinateType, PodString< InternalCoordinateType >::Get() },
      { keys::Dimensionality, std::to_string( Dimensionality ) }
    };
  }

private:

  DisplacementFieldTransformType m_DisplacementFieldTransform;
  ResampleImageFilterPointer m_ResampleImageFilter;

};

} // namespace selx

#endif // selxDisplacementFieldWarperComponent_h