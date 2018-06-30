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

#ifndef selxItkDisplacementFieldSinkComponent_h
#define selxItkDisplacementFieldSinkComponent_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileWriter.h"
#include "itkSmartPointer.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
template< int Dimensionality, class TPixel  >
class ItkDisplacementFieldSinkComponent :
  public SuperElastixComponent<
    Accepting< itkDisplacementFieldInterface< Dimensionality, TPixel > >,
    Providing< SinkInterface > >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkDisplacementFieldSinkComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< itkDisplacementFieldInterface< Dimensionality, TPixel > >,
    Providing< SinkInterface > > Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkDisplacementFieldSinkComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkDisplacementFieldSinkComponent();

  using itkDisplacementFieldInterfaceType = typename itkDisplacementFieldInterface< Dimensionality, TPixel >::Type;
  using itkDisplacementFieldInterfacePointer = typename itkDisplacementFieldInterfaceType::Pointer;

  using ItkDisplacementFieldType = typename itkDisplacementFieldInterfaceType::ItkDisplacementFieldType;
  using ItkDisplacementFieldPointer = typename ItkDisplacementFieldType::Pointer;

  using ItkDisplacementFieldWriterType = itk::ImageFileWriter< ItkDisplacementFieldType >;
  using ItkDisplacementFieldWriterPointer = typename ItkDisplacementFieldWriterType::Pointer;

  using DecoratedWriterType = FileWriterDecorator< ItkDisplacementFieldWriterType >;
  using DecoratedWriterPointer = typename DecoratedWriterType::Pointer;

  // Sink interfaces
  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;
  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;
  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  // Accept interfaces
  virtual int Accept( itkDisplacementFieldInterfacePointer ) override;

  // Base class interfaces
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "ItkDisplacementFieldSink Component"; }

private:

  typename ItkDisplacementFieldType::Pointer m_MiniPipelineOutputDisplacementField;
  typename ItkDisplacementFieldType::Pointer m_NetworkBuilderOutputDisplacementField;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkDisplacementFieldSinkComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};

} //end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkDisplacementFieldSinkComponent.hxx"
#endif
#endif // #define selxItkDisplacementFieldSinkComponent_h
