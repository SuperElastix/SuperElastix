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

#ifndef selxItkToNiftiImageSourceComponent_h
#define selxItkToNiftiImageSourceComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include "selxItkToNiftiImage.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileReader.h"
#include "itkImportImageFilter.h"

#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

namespace selx
{
template<  int Dimensionality, class TPixel >
class ItkToNiftiImageSourceComponent :
  public SuperElastixComponent<
  Accepting<  >,
  Providing< SourceInterface, NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
    NiftyregWarpedImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >,
    NiftyregInputMaskInterface< unsigned char >, NiftyregInputFloatingMaskInterface< unsigned char >>
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkToNiftiImageSourceComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
    Accepting<  >,
    Providing< SourceInterface, NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
      NiftyregWarpedImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >,
      NiftyregInputMaskInterface< unsigned char >, NiftyregInputFloatingMaskInterface< unsigned char >>
    >                                            Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  typedef typename itk::Image< TPixel, Dimensionality >             ItkImageType;
  typedef typename itk::ImageFileReader< ItkImageType >             ItkImageReaderType;
  typedef typename itk::ImportImageFilter< TPixel, Dimensionality > ImportFilterType;
  typedef FileReaderDecorator< ItkImageReaderType >                 DecoratedReaderType;

  using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;

  ItkToNiftiImageSourceComponent( const std::string & name, LoggerImpl & logger );
  ~ItkToNiftiImageSourceComponent();

  // providing interfaces
  std::shared_ptr< nifti_image > GetReferenceNiftiImage() override;
  std::shared_ptr< nifti_image > GetFloatingNiftiImage() override;
  std::shared_ptr< nifti_image > GetWarpedNiftiImage() override;
  std::shared_ptr< nifti_image > GetInputMask() override;
  std::shared_ptr< nifti_image > GetInputFloatingMask() override;

  typename ItkImageDomainType::Pointer GetItkImageDomainFixed() override;

  void SetMiniPipelineInput( itk::DataObject::Pointer ) override;
  AnyFileReader::Pointer GetInputFileReader( void ) override;

  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkToNiftiImageSource Component"; }

private:

  typename ItkImageType::Pointer m_Image;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkToNiftiImageSourceComponent" }, { keys::PixelType, PodString< TPixel >::Get() },
             { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkToNiftiImageSourceComponent.hxx"
#endif
#endif // #define selxItkToNiftiImageSourceComponent_h
