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

#ifndef selxDisplacementFieldMeshWarperComponent_h
#define selxDisplacementFieldMeshWarperComponent_h

#include "selxSuperElastixComponent.h"
#include "selxItkObjectInterfaces.h"

#include "itkDisplacementFieldTransform.h"
#include "itkTransformMeshFilter.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
class ItkDisplacementFieldMeshWarperComponent : public
  SuperElastixComponent<
  Accepting< itkMeshInterface< Dimensionality, CoordRepType >, itkDisplacementFieldInterface< Dimensionality, TPixel > >,
  Providing< itkMeshInterface< Dimensionality, CoordRepType > > >
{
public:
  typedef ItkDisplacementFieldMeshWarperComponent< Dimensionality, TPixel, CoordRepType > Self;
  typedef SuperElastixComponent<
    Accepting< itkMeshInterface< Dimensionality, CoordRepType >, itkDisplacementFieldInterface< Dimensionality, TPixel > >,
    Providing< itkMeshInterface< Dimensionality, CoordRepType > > > Superclass;
  typedef std::shared_ptr< Self > Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldMeshWarperComponent( const std::string & name, LoggerImpl & logger );

  using ItkDisplacementFieldInterfaceType = typename itkDisplacementFieldInterface< Dimensionality, TPixel >::Type;
  using ItkDisplacementFieldInterfacePointer = typename ItkDisplacementFieldInterfaceType::Pointer;

  using ItkDisplacementFieldType = typename ItkDisplacementFieldInterfaceType::ItkDisplacementFieldType;
  using ItkDisplacementFieldPointer = typename ItkDisplacementFieldType::Pointer;

  using ItkMeshInterfaceType = typename itkMeshInterface< Dimensionality, TPixel >::Type;
  typedef typename ItkMeshInterfaceType::Pointer ItkMeshInterfacePointer;

  using ItkMeshType = typename itkMeshInterface< Dimensionality, TPixel >::ItkMeshType;
  using ItkMeshPointer = typename ItkMeshType::Pointer;

  typedef itk::DisplacementFieldTransform< CoordRepType, Dimensionality > ItkDisplacementFieldTransformType;
  typedef typename ItkDisplacementFieldTransformType::Pointer ItkDisplacementFieldTransformPointer;

  typedef itk::TransformMeshFilter< ItkMeshType, ItkMeshType, ItkDisplacementFieldTransformType > ItkTransformMeshFilterType;
  typedef typename ItkTransformMeshFilterType::Pointer ItkTransformMeshFilterPointer;

  // Accept interfaces
  virtual int Accept( ItkDisplacementFieldInterfacePointer ) override;
  virtual int Accept( ItkMeshInterfacePointer ) override;

  // Provide interfaces
  virtual typename ItkMeshType::Pointer GetItkMesh() override;

  // Base methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "Warp a point set based on a deformation field"; };

protected:

  // return the class name and the template arguments to identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return {
      { keys::NameOfClass, "ItkDisplacementFieldMeshWarperComponent" },
      { keys::PixelType, PodString< TPixel >::Get() },
      { keys::CoordRepType, PodString< CoordRepType >::Get() },
      { keys::Dimensionality, std::to_string( Dimensionality ) }
    };
  }

private:

  ItkDisplacementFieldTransformPointer m_DisplacementFieldTransform;
  ItkTransformMeshFilterPointer m_TransformMeshFilter;

};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldMeshWarperComponent.hxx"
#endif

#endif // selxDisplacementFieldMeshWarperComponent_h
