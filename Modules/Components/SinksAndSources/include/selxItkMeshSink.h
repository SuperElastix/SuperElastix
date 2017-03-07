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

#ifndef selxItkMeshSink_h
#define selxItkMeshSink_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkMeshFileWriter.h"
#include "selxAnyFileWriter.h"
#include "selxFileWriterDecorator.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class ItkMeshSinkComponent :
  public SuperElastixComponent<
    Accepting< itkMeshInterface< Dimensionality, TPixel >>,
    Providing< SinkInterface, AfterRegistrationInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkMeshSinkComponent< 
    Dimensionality, TPixel 
  >                                     Self;
  typedef SuperElastixComponent<
    Accepting< itkMeshInterface< Dimensionality, TPixel >>,
    Providing< SinkInterface, AfterRegistrationInterface >
  >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkMeshSinkComponent( const std::string & name );
  virtual ~ItkMeshSinkComponent();

  typedef itk::Mesh< TPixel, Dimensionality >         ItkMeshType;
  typedef typename itk::MeshFileWriter< ItkMeshType > ItkMeshWriterType;
  typedef FileWriterDecorator< ItkMeshWriterType >    DecoratedWriterType;

  virtual int Set( typename itkMeshInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;

  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  virtual void AfterRegistration() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkMeshSink Component"; }

private:

  typename ItkMeshType::Pointer m_MiniPipelineOutputMesh;
  typename ItkMeshType::Pointer m_NetworkBuilderOutputMesh;

  typename itkMeshInterface< Dimensionality, TPixel >::Pointer m_ProvidingGetItkMeshInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkMeshSinkComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkMeshSink.hxx"
#endif
#endif // #define selxItkMeshSink_h
