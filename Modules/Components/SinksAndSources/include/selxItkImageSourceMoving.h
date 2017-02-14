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

#ifndef selxItkImageSourceMoving_h
#define selxItkImageSourceMoving_h

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
class ItkImageSourceMovingComponent :
  public SuperElastixComponent<
  Accepting< >,
  Providing< SourceInterface, itkImageMovingInterface< Dimensionality, TPixel >>
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkImageSourceMovingComponent   Self;
  typedef ComponentBase                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  ItkImageSourceMovingComponent( const std::string & name );
  virtual ~ItkImageSourceMovingComponent();

  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType ItkImageType;
  typedef typename itk::ImageFileReader< ItkImageType >                            ItkImageReaderType;
  typedef FileReaderDecorator< ItkImageReaderType >                                DecoratedReaderType;

  virtual typename ItkImageType::Pointer GetItkImageMoving() override;

  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) override;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkImageSourceMoving Component"; }

private:

  typename ItkImageType::Pointer m_Image;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkImageSourceMovingComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageSourceMoving.hxx"
#endif
#endif // #define selxItkImageSourceMoving_h
