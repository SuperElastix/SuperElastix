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

#ifndef selxDisplacementFieldComposerComponent_h
#define selxDisplacementFieldComposerComponent_h

#include "selxSuperElastixComponent.h"
#include "selxItkObjectInterfaces.h"

#include "selxComposeDisplacementFieldsImageFilter.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
class ItkDisplacementFieldComposerComponent : public
  SuperElastixComponent<
  Accepting< itkDisplacementFieldInterface< Dimensionality, CoordRepType >, itkWarpingDisplacementFieldInterface< Dimensionality, CoordRepType > >,
  Providing< itkDisplacementFieldInterface< Dimensionality, CoordRepType >, UpdateInterface > >
{
public:
  typedef ItkDisplacementFieldComposerComponent< Dimensionality, TPixel, CoordRepType > Self;
  typedef SuperElastixComponent<
    Accepting< itkDisplacementFieldInterface< Dimensionality, CoordRepType >, itkWarpingDisplacementFieldInterface< Dimensionality, CoordRepType > >,
    Providing< itkDisplacementFieldInterface< Dimensionality, CoordRepType >, UpdateInterface > > Superclass;
  typedef std::shared_ptr< Self > Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldComposerComponent( const std::string & name, LoggerImpl & logger );

  using DisplacementFieldInterfaceType = typename itkDisplacementFieldInterface< Dimensionality, CoordRepType >::Type;
  using DisplacementFieldInterfacePointer = typename DisplacementFieldInterfaceType::Pointer;

  using WarpingDisplacementFieldInterfaceType = typename itkWarpingDisplacementFieldInterface< Dimensionality, CoordRepType >::Type;
  using WarpingDisplacementFieldInterfacePointer = typename WarpingDisplacementFieldInterfaceType::Pointer;

  using DisplacementFieldType = typename DisplacementFieldInterfaceType::ItkDisplacementFieldType;
  using DisplacementFieldPointer = typename DisplacementFieldType::Pointer;

  using WarpingDisplacementFieldType = typename WarpingDisplacementFieldInterfaceType::ItkDisplacementFieldType;
  using WarpingDisplacementFieldPointer = typename WarpingDisplacementFieldType::Pointer;

  using ComposeDisplacementFieldsImageFilterType = ComposeDisplacementFieldsImageFilter< DisplacementFieldType, DisplacementFieldType >;
  using ComposeDisplacementFieldsImageFilterPointer = typename ComposeDisplacementFieldsImageFilterType::Pointer;

  // Accept interfaces
  int Accept( DisplacementFieldInterfacePointer ) override;
  int Accept( WarpingDisplacementFieldInterfacePointer ) override;

  // Provide interfaces
  DisplacementFieldPointer GetItkDisplacementField() override;

  // Base methods
  void BeforeUpdate() override;
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "Warp a point set based on a deformation field"; };

protected:

  // return the class name and the template arguments to identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return {
      { keys::NameOfClass, "ItkDisplacementFieldComposerComponent" },
      { keys::PixelType, PodString< TPixel >::Get() },
      { keys::CoordRepType, PodString< CoordRepType >::Get() },
      { keys::Dimensionality, std::to_string( Dimensionality ) }
    };
  }

private:

  ComposeDisplacementFieldsImageFilterPointer m_ComposeDisplacementFieldsImageFilter;
  DisplacementFieldPointer m_DisplacementField;
  WarpingDisplacementFieldPointer m_WarpingDisplacementField;

};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldComposerComponent.hxx"
#endif

#endif // selxDisplacementFieldComposerComponent_h
