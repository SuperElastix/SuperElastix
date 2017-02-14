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

#ifndef selxDisplacementFieldItkImageFilterSink_h
#define selxDisplacementFieldItkImageFilterSink_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileWriter.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class DisplacementFieldItkImageFilterSinkComponent :
  public SuperElastixComponent<
  Accepting< DisplacementFieldItkImageSourceInterface< Dimensionality, TPixel > >,
  Providing< SinkInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef DisplacementFieldItkImageFilterSinkComponent          Self;
  typedef ComponentBase                       Superclass;
  typedef std::shared_ptr< Self >           Pointer;
  typedef std::shared_ptr< const Self >     ConstPointer;


  DisplacementFieldItkImageFilterSinkComponent(const std::string & name);
  virtual ~DisplacementFieldItkImageFilterSinkComponent();

  typedef TPixel PixelType;

  typedef DisplacementFieldItkImageSourceInterface< Dimensionality, TPixel >              AcceptingDisplacementFieldInterfaceType;
  typedef typename itk::Image< itk::Vector< PixelType, Dimensionality >, Dimensionality > DeformationFieldImageType;
  typedef typename DeformationFieldImageType::Pointer                                     DeformationFieldImageTypePointer;
  typedef typename itk::ImageFileWriter< DeformationFieldImageType >                      DeformationFieldImageWriterType;
  typedef FileWriterDecorator< DeformationFieldImageWriterType >                          DecoratedWriterType;

  //Accepting Interfaces:
  virtual int Set( typename AcceptingDisplacementFieldInterfaceType::Pointer ) override;

  //Providing Interfaces:
  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;

  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "DisplacementFieldItkImageFilterSink Component"; }

private:

  typename DeformationFieldImageType::Pointer m_MiniPipelineOutputImage;
  typename DeformationFieldImageType::Pointer m_NetworkBuilderOutputImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "DisplacementFieldItkImageFilterSinkComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldItkImageFilterSink.hxx"
#endif
#endif // #define selxDisplacementFieldItkImageFilterSink_h
