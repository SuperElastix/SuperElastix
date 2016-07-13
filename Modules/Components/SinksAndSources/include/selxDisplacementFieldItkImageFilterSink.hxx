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

#include "selxDisplacementFieldItkImageFilterSink.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::DisplacementFieldItkImageFilterSinkComponent():
    m_MiniPipelineOutputImage(nullptr), m_OverlordOutputImage(nullptr)
  {
  }

  template<int Dimensionality, class TPixel>
  DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::~DisplacementFieldItkImageFilterSinkComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::Set(AcceptingDisplacementFieldInterfaceType* other)
  {
    if (this->m_OverlordOutputImage == nullptr)
    {
      itkExceptionMacro("SinkComponent needs to be initialized by SetMiniPipelineOutput()");
    }

    // Store pointer to MiniPipelineOutputImage for later grafting onto Overlord output.
    this->m_MiniPipelineOutputImage = other->GetDisplacementFieldItkImage();
    // Graft Overlord output onto MiniPipelineOutputImage.
    this->m_MiniPipelineOutputImage->Graft(this->m_OverlordOutputImage);
    return 0;
  }


  template<int Dimensionality, class TPixel>
  void DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::AfterRegistration()
  {
    this->m_MiniPipelineOutputImage->Update();
  }

  template<int Dimensionality, class TPixel>
  typename AnyFileWriter::Pointer DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::GetOutputFileWriter()
  {
    // Instanstiate an image file writer, decorated such that it can be implicitly cast to an AnyFileWriterType
    return DecoratedWriterType::New().GetPointer();
  }

  template<int Dimensionality, class TPixel>
  void DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::SetMiniPipelineOutput(itk::DataObject::Pointer overlordOutput)
  {
    /** Tries to cast the overlordOutput to an image (data object) and stores the result.
    *  The resulting output image will be grafted into when the sink component is connected to an other component.
    * */
    // 
    this->m_OverlordOutputImage = dynamic_cast<DeformationFieldImageType*>(overlordOutput.GetPointer());
    if (this->m_OverlordOutputImage == nullptr)
    {
      itkExceptionMacro("SinkComponent cannot cast the Overlord's Output to the required type");
    }
  }

  template<int Dimensionality, class TPixel>
  typename itk::DataObject::Pointer DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::GetMiniPipelineOutput()
  {
    return this->m_MiniPipelineOutputImage.GetPointer();
  }

  template<int Dimensionality, class TPixel>
  typename itk::DataObject::Pointer DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::GetInitializedOutput()
  {
    return DeformationFieldImageType::New().GetPointer();
  }

  template<int Dimensionality, class TPixel>
  bool DisplacementFieldItkImageFilterSinkComponent< Dimensionality, TPixel>::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
    if (criterion.first == "IsVectorField")
    {
      meetsCriteria = true;
      for (auto const & criterionValue : criterion.second) // auto&& preferred?
      {
        if (criterionValue != "True")  // e.g. "GradientDescent", "SupportsSparseSamples
        {
          meetsCriteria = false;
        }
      }
    }
    return meetsCriteria;
  }

} //end namespace selx
