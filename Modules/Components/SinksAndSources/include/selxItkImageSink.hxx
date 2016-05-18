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

#include "selxItkImageSink.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageSinkComponent< Dimensionality, TPixel>::ItkImageSinkComponent()
  {
    this->m_Image = nullptr;
    //this->m_ProvidingGetItkImageInterface = nullptr;
  }

  template<int Dimensionality, class TPixel>
  ItkImageSinkComponent< Dimensionality, TPixel>::~ItkImageSinkComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int ItkImageSinkComponent< Dimensionality, TPixel>::Set(itkImageInterface<Dimensionality, TPixel>* other)
  {
    if (this->m_Image == nullptr)
    {
      itkExceptionMacro("SinkComponent needs to be initialized by ConnectToOverlordSink()");
    }
    other->GetItkImage()->Graft(this->m_Image);
    return 0;
  }

  //template<int Dimensionality, class TPixel>
  //int ItkImageSinkComponent< Dimensionality, TPixel>::Set(GetItkImageInterface<Dimensionality, TPixel>* other)
  //{
  //  this->m_ProvidingGetItkImageInterface = other;
  //  return 0;
  //}

  template<int Dimensionality, class TPixel>
  void ItkImageSinkComponent< Dimensionality, TPixel>::AfterRegistration()
  {

  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSinkComponent< Dimensionality, TPixel>::ConnectToOverlordSink(itk::DataObject::Pointer object)
  {
    this->m_Image = dynamic_cast<ItkImageType*>(&(*object));

    return (this->m_Image != nullptr);
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSinkComponent< Dimensionality, TPixel>::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
