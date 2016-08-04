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

#include "selxItkImageSourceFixed.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageSourceFixedComponent< Dimensionality, TPixel>
    ::ItkImageSourceFixedComponent() : m_Image(nullptr)
  {
  }

  template<int Dimensionality, class TPixel>
  ItkImageSourceFixedComponent< Dimensionality, TPixel>
  ::~ItkImageSourceFixedComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  typename ItkImageSourceFixedComponent< Dimensionality, TPixel>::ItkImageType::Pointer 
  ItkImageSourceFixedComponent< Dimensionality, TPixel>
  ::GetItkImageFixed()
  {
    if (this->m_Image == nullptr)
    {
      itkExceptionMacro("SourceComponent needs to be initialized by SetMiniPipelineInput()");
    }
    return this->m_Image;
  }

  template<int Dimensionality, class TPixel>
  void 
  ItkImageSourceFixedComponent< Dimensionality, TPixel>
  ::SetMiniPipelineInput(itk::DataObject::Pointer object)
  {
    this->m_Image = dynamic_cast<ItkImageType*>(object.GetPointer());
    if (this->m_Image == nullptr)
    {
      itkExceptionMacro("DataObject passed by the Overlord is not of the right ImageType or not at all an ImageType");
    }
    return;
  }

  template<int Dimensionality, class TPixel>
  typename AnyFileReader::Pointer ItkImageSourceFixedComponent< Dimensionality, TPixel>::GetInputFileReader()
  {
    // Instanstiate an image file reader, decorated such that it can be implicitly cast to an AnyFileReaderType
    return DecoratedReaderType::New().GetPointer();
  }

  template<int Dimensionality, class TPixel>
  typename ItkImageSourceFixedComponent< Dimensionality, TPixel>::ItkImageDomainType::Pointer
    ItkImageSourceFixedComponent< Dimensionality, TPixel>
    ::GetItkImageDomainFixed()
  {
    if (this->m_Image == nullptr)
    {
      itkExceptionMacro("SourceComponent needs to be initialized by SetMiniPipelineInput()");
    }
    return this->m_Image.GetPointer();
  }

  template<int Dimensionality, class TPixel>
  bool 
  ItkImageSourceFixedComponent< Dimensionality, TPixel>
  ::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
