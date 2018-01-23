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

#ifndef selxNiftiToItkImageSinkComponent_h
#define selxNiftiToItkImageSinkComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include "selxNiftiToItkImage.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileWriter.h"
#include "itkImportImageFilter.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class NiftiToItkImageSinkComponent :
  public SuperElastixComponent<
  Accepting< NiftyregWarpedImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >>,
  Providing< SinkInterface, UpdateInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef NiftiToItkImageSinkComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< NiftyregWarpedImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >>,
    Providing< SinkInterface, UpdateInterface >
    >                                            Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  typedef NiftyregWarpedImageInterface< TPixel >         WarpedImageInterfaceType;
  typedef itkImageDomainFixedInterface< Dimensionality > ImageDomainInterfaceType;
  typedef std::shared_ptr< nifti_image >                 NiftiImagePointer;

  typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
  typedef typename itk::ImageFileWriter< ItkImageType > ItkImageWriterType;
  typedef FileWriterDecorator< ItkImageWriterType >     DecoratedWriterType;

  typedef itk::ImportImageFilter< TPixel, Dimensionality > ImportFilterType;

  NiftiToItkImageSinkComponent( const std::string & name, LoggerImpl & logger );
  virtual ~NiftiToItkImageSinkComponent();

  // accepting interfaces
  virtual int Accept( typename WarpedImageInterfaceType::Pointer ) override;

  virtual int Accept( typename ImageDomainInterfaceType::Pointer ) override;

  // prodiving interfaces
  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;

  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  virtual void Update() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "NiftiToItkImageSink Component"; }

private:

  typename WarpedImageInterfaceType::Pointer m_WarpedImageInterface;
  typename ImageDomainInterfaceType::Pointer m_ImageDomainInterface;
  typename ImportFilterType::Pointer m_ImportFilter;
  typename ItkImageType::Pointer m_MiniPipelineOutputImage;
  typename ItkImageType::Pointer m_NetworkBuilderOutputImage;

  //NiftiImagePointer m_MiniPipelineOutputImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "NiftiToItkImageSinkComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftiToItkImageSinkComponent.hxx"
#endif
#endif // #define selxNiftiToItkImageSinkComponent_h
