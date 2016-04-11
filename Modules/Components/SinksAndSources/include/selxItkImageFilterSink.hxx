#include "selxItkImageFilterSink.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageFilterSinkComponent< Dimensionality, TPixel>::ItkImageFilterSinkComponent()
  {
    this->m_Sink = nullptr;
    this->m_SinkWriter = nullptr;
  }

  template<int Dimensionality, class TPixel>
  ItkImageFilterSinkComponent< Dimensionality, TPixel>::~ItkImageFilterSinkComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int ItkImageFilterSinkComponent< Dimensionality, TPixel>::Set(itkImageSourceInterface<Dimensionality, TPixel>* other)
  {
    if (this->m_SinkWriter == nullptr)
    {
      itkExceptionMacro("SinkComponent needs to be initialized by ConnectToOverlordSink()");
    }
    
    this->m_SinkWriter->SetInput(other->GetItkImageSource()->GetOutput());
    return 0;
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageFilterSinkComponent< Dimensionality, TPixel>::ConnectToOverlordSink(itk::Object::Pointer object)
  {
    bool anySuccessfulCast = false;
    this->m_Sink = dynamic_cast<itk::ProcessObject*>(object.GetPointer());
    anySuccessfulCast = this->m_Sink ? true : anySuccessfulCast;
    this->m_SinkWriter = dynamic_cast<itk::ImageFileWriter<itk::Image<TPixel,Dimensionality>>*>(object.GetPointer());
    anySuccessfulCast = this->m_SinkWriter ? true : anySuccessfulCast;
    return anySuccessfulCast;
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageFilterSinkComponent< Dimensionality, TPixel>::MeetsCriterion(const ComponentBase::CriterionType &criterion)
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
