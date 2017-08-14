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

#ifndef selxItkTransformSourceComponent_h
#define selxItkTransformSourceComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>
#include "itkImageFileReader.h"
#include "itkImportImageFilter.h"

#include "selxAnyFileReader.h"
#include "selxItkTransformDataObjectFileReader.h"

namespace selx
{
  template<  int Dimensionality, class InternalComputationValueType >
class ItkTransformSourceComponent :
  public SuperElastixComponent<
  Accepting<  >,
  Providing< SourceInterface, itkTransformInterface< InternalComputationValueType, Dimensionality > >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkTransformSourceComponent< Dimensionality, InternalComputationValueType > Self;
  typedef SuperElastixComponent<
    Accepting<  >,
    Providing < SourceInterface, itkTransformInterface< InternalComputationValueType, Dimensionality > >
    >                                            Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  using TransformInterface = itkTransformInterface< InternalComputationValueType, Dimensionality >;
  using TransformType = typename TransformInterface::TransformType;
  using TransformPointer = typename TransformInterface::TransformPointer;
  using DecoratedTransformType = itk::DataObjectDecorator<TransformType>;
  using DecoratedReaderType = ItkTransformDataObjectFileReader< InternalComputationValueType, Dimensionality, Dimensionality>;

  ItkTransformSourceComponent( const std::string & name, const LoggerInterface & logger );
  virtual ~ItkTransformSourceComponent();

  // providing interfaces
  virtual TransformPointer GetItkTransform() override;

  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) override;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) override;
  
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkToNiftiImageSource Component"; }

private:

  typename DecoratedTransformType::Pointer m_DecoratedTransform;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return{ { keys::NameOfClass, "ItkTransformSourceComponent" }, { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string(Dimensionality) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkTransformSourceComponent.hxx"
#endif
#endif // #define selxItkTransformSourceComponent_h
