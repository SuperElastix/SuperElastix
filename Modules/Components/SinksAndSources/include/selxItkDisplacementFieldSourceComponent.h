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

#ifndef selxItkDisplacementFieldSourceComponent_h
#define selxItkDisplacementFieldSourceComponent_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileReader.h"
#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

namespace selx
{
template<  int Dimensionality, class TPixel >
class ItkDisplacementFieldSourceComponent :
  public SuperElastixComponent<
    Accepting< >,
    Providing< SourceInterface,
      itkDisplacementFieldInterface< Dimensionality, TPixel >,
      itkWarpingDisplacementFieldInterface< Dimensionality, TPixel >,
      itkImageDomainFixedInterface< Dimensionality > > >
{
public:
  /** Standard ITK typedefs. */
  typedef ItkDisplacementFieldSourceComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< >,
    Providing<
      SourceInterface,
      itkDisplacementFieldInterface< Dimensionality, TPixel >,
      itkWarpingDisplacementFieldInterface< Dimensionality, TPixel >,
      itkImageDomainFixedInterface< Dimensionality > > > Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldSourceComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkDisplacementFieldSourceComponent();

  using ItkDisplacementFieldType = typename itkDisplacementFieldInterface< Dimensionality, TPixel >::ItkDisplacementFieldType;
  using ItkDisplacementFieldPointer = typename ItkDisplacementFieldType::Pointer;

  using ItkWarpingDisplacementFieldType = typename itkWarpingDisplacementFieldInterface< Dimensionality, TPixel >::ItkDisplacementFieldType;
  using ItkWarpingDisplacementFieldPointer = typename ItkWarpingDisplacementFieldType::Pointer;

  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;
  using ItkImageDomainPointer = typename ItkImageDomainType::Pointer;

  using ItkDisplacementFieldReaderType = typename itk::ImageFileReader< ItkDisplacementFieldType >;
  using ItkDisplacementFieldReaderPointer = typename ItkDisplacementFieldReaderType::Pointer;
  typedef FileReaderDecorator< ItkDisplacementFieldReaderType > DecoratedReaderType;

  // Source interface
  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) override;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) override;

  // Providing interfaces
  virtual ItkImageDomainPointer GetItkImageDomainFixed() override;
  virtual ItkDisplacementFieldPointer GetItkDisplacementField() override;

  // Base class methods
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "ItkDisplacementFieldSource Component"; }

private:

  typename ItkDisplacementFieldType::Pointer m_DisplacementField;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkDisplacementFieldSourceComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkDisplacementFieldSourceComponent.hxx"
#endif
#endif // #define selxItkDisplacementFieldSourceComponent_h
