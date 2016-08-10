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

#include "selxMonolithicTransformix.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  MonolithicTransformixComponent< Dimensionality, TPixel>::MonolithicTransformixComponent()
  {
    m_transformixFilter = TransformixFilterType::New();


    m_transformixFilter->ComputeDeformationFieldOn();
    m_transformixFilter->LogToConsoleOn();
    m_transformixFilter->LogToFileOff();
    m_transformixFilter->SetOutputDirectory(".");

    //TODO m_elastixFilter returns a nullptr GetTransformParameterObject instead of a valid object. However, we need this object to satisfy the input conditions of m_transformixFilter
    elxParameterObjectPointer elxParameterObject = elxParameterObjectType::New();
    typename elxParameterObjectType::ParameterMapType defaultParameters = elxParameterObject->GetDefaultParameterMap("rigid");
    elxParameterObject->SetParameterMap(defaultParameters);
    //m_transformixFilter->SetTransformParameterObject(m_elastixFilter->GetTransformParameterObject());
    m_transformixFilter->SetTransformParameterObject(elxParameterObject); // supply a dummy object

    //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
    // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
  }

  template<int Dimensionality, class TPixel>
  MonolithicTransformixComponent< Dimensionality, TPixel>::~MonolithicTransformixComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int MonolithicTransformixComponent< Dimensionality, TPixel>::Set(itkImageDomainFixedInterface<Dimensionality>* component)
  {
    auto fixedImageDomain = component->GetItkImageDomainFixed();
    // Currently, the fixed image domain is part of the transformParameter map, which will be set by elastix.
    // TODO:  make this component use the provided domain
    
    return 0;
  }

  template<int Dimensionality, class TPixel>
  int MonolithicTransformixComponent< Dimensionality, TPixel>::Set(itkImageMovingInterface<Dimensionality, TPixel>* component)
  {
    auto movingImage = component->GetItkImageMoving();
    // connect the itk pipeline
    this->m_transformixFilter->SetInput(movingImage);
    return 0;
  }

  template<int Dimensionality, class TPixel>
  int MonolithicTransformixComponent< Dimensionality, TPixel>::Set(elastixTransformParameterObjectInterfaceType* component)
  {

    auto transformParameterObject = component->GetTransformParameterObject();
    // connect the itk pipeline
    // Due to the fact that elastixfilter returns a Null object we cannot use it as a pipeline
    //this->m_transformixFilter->SetTransformParameterObject(transformParameterObject);
    // Therefore store the interface for the ReconnectTransform call
    this->m_TransformParameterObjectInterface = component;
    return 0;
  }

  //Since elastixFilter is not a true itkfilter we cannot use itkImageSourceInterface (yet)
 
  template<int Dimensionality, class TPixel>
  typename MonolithicTransformixComponent< Dimensionality, TPixel>::ResultImageType::Pointer MonolithicTransformixComponent< Dimensionality, TPixel>::GetItkImage()
  {
    return this->m_transformixFilter->GetOutput();
  }
  


  template<int Dimensionality, class TPixel>
  void MonolithicTransformixComponent< Dimensionality, TPixel>::ReconnectTransform(void)
  {
    // TODO currently, the pipeline with elastix and tranformix can only be created after the update of elastix
    this->m_transformixFilter->SetTransformParameterObject(this->m_TransformParameterObjectInterface->GetTransformParameterObject());
  }


  template<int Dimensionality, class TPixel>
  bool
    MonolithicTransformixComponent< Dimensionality, TPixel>
    ::MeetsCriterion(const CriterionType &criterion)
  {
    bool hasUndefinedCriteria(false);
    bool meetsCriteria(false);
    if (criterion.first == "ComponentProperty")
    {
      meetsCriteria = true;
      for (auto const & criterionValue : criterion.second) // auto&& preferred?
      {
        if (criterionValue != "SomeProperty")  // e.g. "GradientDescent", "SupportsSparseSamples
        {
          meetsCriteria = false;
        }
      }
    }
    else if (criterion.first == "Dimensionality") //Supports this?
    {
      meetsCriteria = true;
      for (auto const & criterionValue : criterion.second) // auto&& preferred?
      {
        if (std::stoi(criterionValue) != Dimensionality)
        {
          meetsCriteria = false;
        }
      }

    }
    else if (criterion.first == "PixelType") //Supports this?
    {
      meetsCriteria = true;
      for (auto const & criterionValue : criterion.second) // auto&& preferred?
      {
        if (criterionValue != Self::GetPixelTypeNameString())
        {
          meetsCriteria = false;
        }
      }

    }
    return meetsCriteria;
  }

} //end namespace selx
