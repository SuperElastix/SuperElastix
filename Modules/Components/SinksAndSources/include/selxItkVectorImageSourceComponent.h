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

#ifndef selxItkVectorImageSourceComponent_h
#define selxItkVectorImageSourceComponent_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileReader.h"
#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class ItkVectorImageSourceComponent :
  public SuperElastixComponent<
    Accepting< >,
    Providing< SourceInterface,
      itkDisplacementFieldInterface< TPixel, Dimension >,
      itkImageDomainFixedInterface< Dimension > > >
{
public:
  /** Standard ITK typedefs. */
  typedef ItkVectorImageSourceComponent< TPixel, Dimension > Self;
  typedef SuperElastixComponent<
    Accepting< >,
    Providing<
      SourceInterface,
      itkDisplacementFieldInterface< TPixel, Dimension >,
      itkImageDomainFixedInterface< Dimension > > > Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkVectorImageSourceComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkVectorImageSourceComponent();

  using ItkVectorImageType = itkDisplacementFieldInterface< TPixel, Dimension >::ItkVectorImageType;
  using ItkVectorImagePointer = typename ItkVectorImageType::Pointer;

  using ItkVectorImageReaderType = itk::ImageFileReader< ItkVectorImageType >;
  using ItkVectorImageReaderPointer = typename ItkVectorImageReaderType::Pointer;
  typedef FileReaderDecorator< ItkVectorImageReaderType > DecoratedReaderType;

  // Providing interfaces
  virtual typename ItkVectorImageType::Pointer GetItkDisplacementField()

  virtual typename ItkImageDomainType::Pointer GetItkImageDomainFixed() override;

  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) override;

  virtual AnyFileReader::Pointer GetInputFileReader( void ) override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkVectorImageSource Component"; }

private:

  typename ItkVectorImageType::Pointer m_VectorImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkVectorImageSourceComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkVectorImageSourceComponent.hxx"
#endif
#endif // #define selxItkVectorImageSourceComponent_h
