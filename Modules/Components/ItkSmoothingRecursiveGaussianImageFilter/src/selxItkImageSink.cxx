#include "selxItkImageSink.h"

namespace selx
{

  ItkImageSinkComponent::ItkImageSinkComponent()
  {
    this->m_Sink = nullptr;
    this->m_SinkWriter = nullptr;
  }

  ItkImageSinkComponent::~ItkImageSinkComponent()
  {
  }

  int ItkImageSinkComponent::Set(itkImageSourceInterface* other)
  {
    if (this->m_SinkWriter == nullptr)
    {
      itkExceptionMacro("SinkComponent needs to be initialized by ConnectToOverlordSink()");
    }
    
    this->m_SinkWriter->SetInput(other->GetItkImageSource()->GetOutput());
    return 0;
  }

  bool ItkImageSinkComponent::ConnectToOverlordSink(itk::Object::Pointer object)
  {
    bool anySuccessfulCast = false;
    this->m_Sink = dynamic_cast<itk::ProcessObject*>(object.GetPointer());
    anySuccessfulCast = this->m_Sink ? true : anySuccessfulCast;
    this->m_SinkWriter = dynamic_cast<itk::ImageFileWriter<itk::Image<double, 3>>*>(object.GetPointer());
    anySuccessfulCast = this->m_SinkWriter ? true : anySuccessfulCast;
    return anySuccessfulCast;
  }


  bool ItkImageSinkComponent::MeetsCriterion(const CriterionType &criterion)
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
    return meetsCriteria;
  }

} //end namespace selx
