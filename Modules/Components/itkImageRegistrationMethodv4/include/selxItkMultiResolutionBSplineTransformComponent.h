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

#ifndef selxItkMultiResolutionBSplineTransformComponent_h
#define selxItkMultiResolutionBSplineTransformComponent_h

#include "selxSuperElastixComponent.h"
#include "selxAccepting.h"
#include "selxProviding.h"
#include "selxItkObjectInterfaces.h" // For itkImageDomainFixedInterface

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkBSplineTransform.h"

namespace selx
{
  template< class InternalComputationValueType, int Dimensionality >
  class ItkMultiResolutionBSplineTransformComponent :
    public SuperElastixComponent<
    Accepting< itkImageDomainFixedInterface< Dimensionality >>,
    Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
    itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality > >
    >
  {
  public:
    using itkTransformParametersAdaptorsContainerInterfaceType
      = itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality >;
    using TransformParametersAdaptorsContainerType
      = typename itkTransformParametersAdaptorsContainerInterfaceType::TransformParametersAdaptorsContainerType;

    //Providing Interfaces:
    virtual TransformParametersAdaptorsContainerType GetItkTransformParametersAdaptorsContainer() override;


    /** Standard ITK typedefs. */
    typedef ItkMultiResolutionBSplineTransformComponent<
      InternalComputationValueType, Dimensionality
    >                                       Self;
    typedef SuperElastixComponent<
      Accepting< itkImageDomainFixedInterface< Dimensionality >>,
      Providing< itkTransformInterface< InternalComputationValueType, Dimensionality >,
      itkTransformParametersAdaptorsContainerInterface< InternalComputationValueType, Dimensionality > >
    >                                       Superclass;
    typedef std::shared_ptr< Self >       Pointer;
    typedef std::shared_ptr< const Self > ConstPointer;

    ItkMultiResolutionBSplineTransformComponent(const std::string & name, LoggerImpl & logger);
    virtual ~ItkMultiResolutionBSplineTransformComponent();

    /** Get types from interfaces */
    using TransformType = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformType;
    using TransformPointer = typename itkTransformInterface< InternalComputationValueType, Dimensionality >::TransformPointer;
    using ItkImageDomainType = typename itkImageDomainFixedInterface< Dimensionality >::ItkImageDomainType;

    typedef itk::Array< itk::SizeValueType >                    ShrinkFactorsArrayType;
    typedef itk::Vector<double, Dimensionality> FinalGridSpacingInPhysicalUnitsArrayType;


    using BSplineTransformType
      = typename itk::BSplineTransform< InternalComputationValueType, Dimensionality >;

    //Accepting Interfaces:
    virtual int Accept(typename itkImageDomainFixedInterface< Dimensionality >::Pointer) override;

    //Providing Interfaces:
    virtual TransformPointer GetItkTransform() override;

    //BaseClass methods
    virtual bool MeetsCriterion(const ComponentBase::CriterionType & criterion) override;

    static const char * GetDescription() { return "ItkMultiResolutionBSplineTransform Component"; }

  private:

    typename BSplineTransformType::Pointer m_Transform;
    typename ItkImageDomainType::Pointer m_FixedImageDomain;
    ShrinkFactorsArrayType m_shrinkFactorsPerLevel;
    TransformParametersAdaptorsContainerType m_adaptors;
    std::vector<typename BSplineTransformType::MeshSizeType> m_meshSizes;

    unsigned m_NumberOfLevels = 0;


  protected:

    // return the class name and the template arguments to uniquely identify this component.
    static inline const std::map< std::string, std::string > TemplateProperties()
    {
      return{ { keys::NameOfClass, "ItkMultiResolutionBSplineTransformComponent" },{ keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() },{ keys::Dimensionality, std::to_string(Dimensionality) } };
    }
  };

  template< class InternalComputationValueType, int Dimensionality >
  auto selx::ItkMultiResolutionBSplineTransformComponent<InternalComputationValueType, Dimensionality>
    ::GetItkTransformParametersAdaptorsContainer() ->TransformParametersAdaptorsContainerType
  {
    return this->m_adaptors;
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkMultiResolutionBSplineTransformComponent.hxx"
#endif
#endif // #define selxItkMultiResolutionBSplineTransformComponent_h
