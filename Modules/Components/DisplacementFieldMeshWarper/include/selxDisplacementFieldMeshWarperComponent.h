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
#include "itkTransformMeshFilter.h"

//
// Accepting< ItkVectorImageInterface< Dimensionality, TPixel >, ItkMeshInterface< Dimensionality, TCoordinateType > >,
//Providing< ItkMeshInterface< Dimensionality, TCoordinateType > >

namespace selx {

template< int Dimensionality, class TPixel, class TCoordRepType >
class ItkDisplacementFieldMeshWarperComponent : public
  SuperElastixComponent<
    Accepting< itkMeshInterface< Dimensionality, TCoordRepType >, itkDisplacementFieldInterface< Dimensionality, TPixel > >,
    Providing< itkMeshInterface< Dimensionality, TCoordRepType > > >
{
public:
  typedef ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, TCoordRepType > Self;
  typedef SuperElastixComponent<
    Accepting< itkMeshInterface< Dimensionality, TCoordRepType >, itkDisplacementFieldInterface< Dimensionality, TPixel > >,
    Providing< itkMeshInterface< Dimensionality, TCoordRepType > > > Superclass;
  typedef std::shared_ptr< Self > Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldMeshWarperComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkDisplacementFieldMeshWarperComponent();

  using ItkDisplacementFieldInterfaceType = typename itkDisplacementFieldInterface< Dimensionality, TPixel >::Type;
  using ItkDisplacementFieldInterfacePointer = typename ItkDisplacementFieldInterfaceType::Pointer;

  using ItkVectorImageType = typename ItkDisplacementFieldInterfaceType::ItkVectorImageType;
  using ItkVectorImagePointer = typename ItkVectorImageType::Pointer;

  using ItkMeshInterfaceType = typename itkMeshInterface< Dimensionality, TPixel >::Type;
  typedef typename ItkMeshInterfaceType::Pointer ItkMeshInterfacePointer;

  using ItkMeshType = typename itkMeshInterface< Dimensionality, TPixel >::ItkMeshType;
  using ItkMeshPointer = typename ItkMeshType::Pointer;

  typedef itk::DisplacementFieldTransform< TCoordRepType, Dimensionality > ItkDisplacementFieldTransformType;
  typedef typename ItkDisplacementFieldTransformType::Pointer ItkDisplacementFieldTransformPointer ;

  virtual int Accept( ItkDisplacementFieldInterfaceType * );
  virtual int Accept( ItkMeshInterfaceType * );

  ItkMeshType * GetWarpedItkMesh();

  static const char * GetDescription() { return "Warp a point set based on a deformation field"; };

protected:

  // return the class name and the template arguments to identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return {
      { keys::NameOfClass, "ItkDisplacementFieldMeshWarperComponent" },
      { keys::PixelType, PodString< TPixel >::Get() },
      { keys::TCoordRepType, PodString< TCoordRepType >::Get() },
      { keys::Dimensionality, std::to_string( Dimensionality ) }
    };
  }

private:

  ItkDisplacementFieldTransformPointer m_DisplacementFieldTransform;

};

} // namespace selx

#endif // selxDisplacementFieldWarperComponent_h