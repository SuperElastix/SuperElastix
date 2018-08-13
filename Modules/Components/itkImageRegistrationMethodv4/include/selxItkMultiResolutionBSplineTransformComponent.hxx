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

#include "selxItkMultiResolutionBSplineTransformComponent.h"
#include "itkBSplineTransformInitializer.h"

#include <algorithm>  // For max.
#include <regex>
#include "itkBSplineTransformParametersAdaptor.h"

namespace selx
{

  template< class InternalComputationValueType, int Dimensionality >
  ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType, Dimensionality >::ItkMultiResolutionBSplineTransformComponent(
    const std::string & name,
    LoggerImpl & logger)
    :
    Superclass{ name, logger },
    m_Transform{ BSplineTransformType::New() }
  {
  }


  template< class InternalComputationValueType, int Dimensionality >
  ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType,
    Dimensionality >::~ItkMultiResolutionBSplineTransformComponent() = default;


  template< class InternalComputationValueType, int Dimensionality >
  int
    ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType, Dimensionality >
    ::Accept(typename itkImageDomainFixedInterface< Dimensionality >::Pointer component)
  {
    this->m_FixedImageDomain = component->GetItkImageDomainFixed();

    const auto transformInitializer = itk::BSplineTransformInitializer <BSplineTransformType, itk::ImageBase<Dimensionality>>::New();
    transformInitializer->SetImage(m_FixedImageDomain);
    transformInitializer->SetTransform(m_Transform);
    transformInitializer->InitializeTransform();

    for (unsigned level = 0; level < m_NumberOfLevels; ++level)
    {
      const auto adaptor = itk::BSplineTransformParametersAdaptor<BSplineTransformType>::New();

      adaptor->SetRequiredTransformDomainMeshSize(m_meshSizes[level]);
      adaptor->SetRequiredTransformDomainOrigin(m_Transform->GetTransformDomainOrigin());
      adaptor->SetRequiredTransformDomainDirection(m_Transform->GetTransformDomainDirection());
      adaptor->SetRequiredTransformDomainPhysicalDimensions(m_Transform->GetTransformDomainPhysicalDimensions());
      m_adaptors[level] = adaptor;
    }

    return 0;
  }


  template< class InternalComputationValueType, int Dimensionality >
  typename ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType, Dimensionality >::TransformPointer
    ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType, Dimensionality >::GetItkTransform()
  {
    return this->m_Transform.GetPointer();
  }

  template< class InternalComputationValueType, int Dimensionality >
  bool
    ItkMultiResolutionBSplineTransformComponent< InternalComputationValueType, Dimensionality >
    ::MeetsCriterion(const ComponentBase::CriterionType & criterion)
  {
    const auto status = CheckTemplateProperties(this->TemplateProperties(), criterion);

    if (status == CriterionStatus::Satisfied)
    {
      return true;
    }
    else if (status == CriterionStatus::Failed)
    {
      return false;
    }
    else
    {
      assert(status == CriterionStatus::Unknown);

      if (criterion.first == "NumberOfLevels")
      {
        assert(m_NumberOfLevels == 0);
        m_NumberOfLevels = std::stoul(criterion.second.front());

        if (m_meshSizes.size() > m_NumberOfLevels)
        {
          m_Logger.Log(LogLevel::DBG, "Too many mesh sizes for this number of levels");
        }
        m_meshSizes.resize(m_NumberOfLevels);
        m_adaptors.resize(m_NumberOfLevels);

        return true;
      }

      std::smatch matchResult;

      if (std::regex_search(criterion.first, matchResult, std::regex{ "Resolution([0-9]+)MeshSize" }))
      {
        if (criterion.second.size() == Dimensionality)
        {
          typename BSplineTransformType::MeshSizeType meshSize = {};

          std::transform(criterion.second.cbegin(), criterion.second.cend(), meshSize.m_Size,
            [](const std::string& str) { return std::stoull(str); });

          const auto resolutionNumber = std::stoull(matchResult[1]);

          if ((m_NumberOfLevels != 0) && (resolutionNumber >= m_NumberOfLevels))
          {
            m_Logger.Log(LogLevel::DBG, "Too many mesh sizes for this number of levels");
          }
          m_meshSizes.resize(std::max<std::size_t>(m_meshSizes.size(), resolutionNumber + 1));
          m_meshSizes[resolutionNumber] = meshSize;

          return true;
        }
        else
        {
          
        }
      }
    }
    return false;
  }
} //end namespace selx
