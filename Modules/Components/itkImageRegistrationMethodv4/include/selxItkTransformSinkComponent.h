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

#ifndef selxItkTransformSinkComponent_h
#define selxItkTransformSinkComponent_h

#include "selxSuperElastixComponent.h"
#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkTransformBase.h"
#include "itkDataObjectDecorator.h"
#include "itkTransformFileWriter.h"
#include "itkSmartPointer.h"
#include "selxAnyFileWriter.h"
#include "selxItkTransfromDataObjectFileWriter.h"

namespace selx
{
template< int Dimensionality, class InternalComputationValueType >
class ItkTransformSinkComponent :
  public SuperElastixComponent<
  Accepting< itkTransformInterface< InternalComputationValueType, Dimensionality >>,
  Providing< SinkInterface, AfterRegistrationInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkTransformSinkComponent<
    Dimensionality, InternalComputationValueType
    >                                     Self;
  typedef SuperElastixComponent<
    Accepting< itkTransformInterface< InternalComputationValueType, Dimensionality >>,
    Providing< SinkInterface, AfterRegistrationInterface >
    >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkTransformSinkComponent( const std::string & name, LoggerImpl & logger );
  virtual ~ItkTransformSinkComponent();

  typedef itkTransformInterface< InternalComputationValueType, Dimensionality >        AcceptingTransformInterfaceType;
  typedef typename AcceptingTransformInterfaceType::TransformType TransformType;
  typedef typename itk::DataObjectDecorator<TransformType> DecoratedTransformType;
  typedef typename TransformType::Pointer                     ItkTransformPointer;
  //typedef typename itk::TransformFileWriterTemplate< InternalComputationValueType >      ItkTransformWriterType;
  typedef ItkTransfromDataObjectFileWriter< InternalComputationValueType, Dimensionality, Dimensionality> DecoratedWriterType;

  virtual int Set( typename AcceptingTransformInterfaceType::Pointer ) override;

  virtual void SetMiniPipelineOutput( itk::DataObject::Pointer ) override;
  virtual itk::DataObject::Pointer GetMiniPipelineOutput( void ) override;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) override;

  virtual itk::DataObject::Pointer GetInitializedOutput( void ) override;

  virtual void AfterRegistration() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkTransformSink Component"; }

private:

  typename DecoratedTransformType::Pointer m_MiniPipelineOutputImage;
  //typename ItkImageType::Pointer m_NetworkBuilderOutputImage;
  typename AcceptingTransformInterfaceType::Pointer m_TransformInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkTransformSinkComponent" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkTransformSinkComponent.hxx"
#endif
#endif // #define selxItkTransformSinkComponent_h
