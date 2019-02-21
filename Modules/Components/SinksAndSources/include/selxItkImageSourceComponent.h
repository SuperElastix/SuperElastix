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

#ifndef selxItkImageSourceComponent_h
#define selxItkImageSourceComponent_h

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
class ItkImageSourceComponent :
  public SuperElastixComponent<
  Accepting< >,
  Providing< SourceInterface,
    itkImageInterface< Dimensionality, TPixel >,
    itkImageMaskInterface< Dimensionality, TPixel >,
    itkImageFixedInterface< Dimensionality, TPixel >,
    itkImageMovingInterface< Dimensionality, TPixel >,
    itkImageDomainFixedInterface< Dimensionality >,
    itkImageMovingMaskInterface< Dimensionality, TPixel >,
    itkImageFixedMaskInterface< Dimensionality, TPixel >>
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkImageSourceComponent<
    Dimensionality, TPixel
    >                                       Self;
  typedef SuperElastixComponent<
    Accepting< >,
    Providing< SourceInterface,
      itkImageInterface< Dimensionality, TPixel >,
      itkImageMaskInterface< Dimensionality, TPixel >,
      itkImageFixedInterface< Dimensionality, TPixel >,
      itkImageMovingInterface< Dimensionality, TPixel >,
      itkImageDomainFixedInterface< Dimensionality >,
      itkImageMovingMaskInterface< Dimensionality, TPixel >,
      itkImageFixedMaskInterface< Dimensionality, TPixel >>
    >                                       Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkImageSourceComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkImageSourceComponent();

  typedef itk::Image< TPixel, Dimensionality >                                        ItkImageType;
  typedef typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType ItkImageDomainType;

  typedef typename itk::ImageFileReader< ItkImageType > ItkImageReaderType;
  typedef FileReaderDecorator< ItkImageReaderType >     DecoratedReaderType;

  // providing interfaces
  typename ItkImageType::Pointer GetItkImage() override;
  typename ItkImageType::Pointer GetItkImageFixed() override;
  typename ItkImageType::Pointer GetItkImageMoving() override;
  typename ItkImageDomainType::Pointer GetItkImageDomainFixed() override;
  typename ItkImageType::Pointer GetItkImageMask() override;
  typename ItkImageType::Pointer GetItkImageFixedMask() override;
  typename ItkImageType::Pointer GetItkImageMovingMask() override;

  void SetMiniPipelineInput( itk::DataObject::Pointer ) override;
  AnyFileReader::Pointer GetInputFileReader( void ) override;

  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkImageSource Component"; }

private:

  typename ItkImageType::Pointer m_Image;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkImageSourceComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageSourceComponent.hxx"
#endif
#endif // #define selxItkImageSource_h
