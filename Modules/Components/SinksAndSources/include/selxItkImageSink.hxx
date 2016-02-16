#include "selxItkImageSink.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageSinkComponent< Dimensionality, TPixel>::ItkImageSinkComponent()
  {
    this->m_Sink = nullptr;
    this->m_SinkWriter = nullptr;
    this->m_ProvidingGetItkImageInterface = nullptr;
  }

  template<int Dimensionality, class TPixel>
  ItkImageSinkComponent< Dimensionality, TPixel>::~ItkImageSinkComponent()
  {
  }

  template<int Dimensionality, class TPixel>
  int ItkImageSinkComponent< Dimensionality, TPixel>::Set(itkImageSourceInterface<Dimensionality, TPixel>* other)
  {
    if (this->m_SinkWriter == nullptr)
    {
      itkExceptionMacro("SinkComponent needs to be initialized by ConnectToOverlordSink()");
    }
    
    this->m_SinkWriter->SetInput(other->GetItkImageSource()->GetOutput());
    return 0;
  }

  template<int Dimensionality, class TPixel>
  int ItkImageSinkComponent< Dimensionality, TPixel>::Set(GetItkImageInterface<Dimensionality, TPixel>* other)
  {
    this->m_ProvidingGetItkImageInterface = other;
    return 0;
  }

  template<int Dimensionality, class TPixel>
  void ItkImageSinkComponent< Dimensionality, TPixel>::AfterRegistration()
  {
    if (this->m_ProvidingGetItkImageInterface) // Set(GetItkImageInterface<Dimensionality, TPixel>* other) was used
    {
      this->m_SinkWriter->SetInput(this->m_ProvidingGetItkImageInterface->GetItkImage());
    }
    else // Set(itkImageSourceInterface<Dimensionality, TPixel>* other) was used
    {
      // A call to AfterRegistration doesn't need to update anything, since this is handled by the itk pipeline
      // Todo check if indeed Set(itkImageSourceInterface<Dimensionality, TPixel>* other) was used
      //
    }
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSinkComponent< Dimensionality, TPixel>::ConnectToOverlordSink(itk::Object::Pointer object)
  {
    bool anySuccessfulCast = false;
    this->m_Sink = dynamic_cast<itk::ProcessObject*>(object.GetPointer());
    anySuccessfulCast = this->m_Sink ? true : anySuccessfulCast;
    this->m_SinkWriter = dynamic_cast<itk::ImageFileWriter<itk::Image<TPixel,Dimensionality>>*>(object.GetPointer());
    anySuccessfulCast = this->m_SinkWriter ? true : anySuccessfulCast;
    return anySuccessfulCast;
  }

  template<int Dimensionality, class TPixel>
  bool ItkImageSinkComponent< Dimensionality, TPixel>::MeetsCriterion(const CriterionType &criterion)
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
        if (std::stoi(criterionValue) != Self::Dimensionality)
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
