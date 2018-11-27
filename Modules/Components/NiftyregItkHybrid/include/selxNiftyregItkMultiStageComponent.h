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

#ifndef selxItkCompositeTransformComponent_h
#define selxItkCompositeTransformComponent_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxNiftyregInterfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkCompositeTransform.h"
#include "_reg_aladin.h"

namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
class NiftyregItkMultiStageComponent :
  public SuperElastixComponent<
    Accepting< NiftyregAffineMatrixInterface, 
	  MultiStageTransformInterface< InternalComputationValueType, Dimensionality >,
      UpdateInterface
    >,
    Providing<
      itkTransformInterface< InternalComputationValueType, Dimensionality >,
      UpdateInterface
    >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef NiftyregItkMultiStageComponent<
    InternalComputationValueType, Dimensionality
    >                                       Self;
  typedef SuperElastixComponent<
    Accepting< NiftyregAffineMatrixInterface,
	  MultiStageTransformInterface< InternalComputationValueType, Dimensionality >,
      UpdateInterface
      >,
      Providing<
        itkTransformInterface< InternalComputationValueType, Dimensionality >,
        UpdateInterface
	  >
    >                                       Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  NiftyregItkMultiStageComponent( const std::string & name, LoggerImpl & logger );
  virtual ~NiftyregItkMultiStageComponent();

  /**  Type of the optimizer. */
  using TransformType = typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::TransformBaseType;

  typedef typename itk::CompositeTransform< InternalComputationValueType, Dimensionality > CompositeTransformType;

  virtual int Accept( typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::Pointer ) override;

  virtual int Accept( typename NiftyregAffineMatrixInterface::Pointer ) override;

  virtual int Accept( UpdateInterface::Pointer ) override;

  virtual void Update() override;

  virtual typename TransformType::Pointer GetItkTransform() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  virtual bool ConnectionsSatisfied() override;

  static const char * GetDescription() { return "NiftyregItkMultiStageComponent Component"; }

private:

  typename CompositeTransformType::Pointer m_CompositeTransform;

  typename NiftyregAffineMatrixInterface::Pointer m_NiftyregAffineMatrixInterface;

  std::map< std::string, typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::Pointer > m_RegistrationStages;
  std::map< std::string, typename UpdateInterface::Pointer > m_UpdateInterfaces;
  std::vector< std::string >                       m_ExecutionOrder;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "NiftyregItkMultiStageComponent" }, { keys::PixelType, PodString< InternalComputationValueType >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftyregItkMultiStageComponent.hxx"
#endif
#endif // #define selxItkCompositeTransformComponent_h
