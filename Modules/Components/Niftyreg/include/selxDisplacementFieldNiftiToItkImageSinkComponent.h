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

#ifndef selxDisplacementFieldNiftiToItkImageSinkComponent_h
#define selxDisplacementFieldNiftiToItkImageSinkComponent_h

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
class DisplacementFieldNiftiToItkImageSinkComponent :
  public SuperElastixComponent<
  Accepting< NiftyregDisplacementFieldImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >>,
  Providing< SinkInterface, AfterRegistrationInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef DisplacementFieldNiftiToItkImageSinkComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< NiftyregDisplacementFieldImageInterface< TPixel >, itkImageDomainFixedInterface< Dimensionality >>,
    Providing< SinkInterface, AfterRegistrationInterface >
    >                                            Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  typedef itkImageDomainFixedInterface< Dimensionality > ItkImageDomainInterfaceType;
  typedef std::shared_ptr< nifti_image >                 NiftiImagePointer;

  typedef NiftyregDisplacementFieldImageInterface< TPixel >             NiftiDisplacementFieldInterfaceType;
  typedef itk::Image< itk::Vector< TPixel, Dimensionality >, Dimensionality >    ItkDisplacementFieldType;
  typedef typename ItkDisplacementFieldType::Pointer                                     ItkDisplacementFieldPointer;
  typedef itk::ImageFileWriter< ItkDisplacementFieldType >                      ItkDisplacementFieldWriterType;
  typedef FileWriterDecorator< ItkDisplacementFieldWriterType >                          DecoratedWriterType;

  typedef itk::ImportImageFilter< itk::Vector< TPixel, Dimensionality >, Dimensionality > ImportFilterType;

  DisplacementFieldNiftiToItkImageSinkComponent( const std::string & name, LoggerImpl & logger );
  virtual ~DisplacementFieldNiftiToItkImageSinkComponent();


  
  // accepting interfaces
  virtual int Accept( typename NiftiDisplacementFieldInterfaceType::Pointer ) override;

  virtual int Accept( typename ItkImageDomainInterfaceType::Pointer ) override;

  // providing interfaces
  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;

  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  virtual void AfterRegistration() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "DisplacementFieldNiftiToItkImageSink Component"; }

private:

  typename NiftiDisplacementFieldInterfaceType::Pointer m_DisplacementFieldInterface;
  typename ItkImageDomainInterfaceType::Pointer m_ImageDomainInterface;
  typename ImportFilterType::Pointer m_ImportFilter;
  typename ItkDisplacementFieldType::Pointer m_MiniPipelineOutputImage;
  typename ItkDisplacementFieldType::Pointer m_NetworkBuilderOutputImage;

  //NiftiImagePointer m_MiniPipelineOutputImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "DisplacementFieldNiftiToItkImageSinkComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.hxx"
#endif
#endif // #define selxDisplacementFieldNiftiToItkImageSinkComponent_h
