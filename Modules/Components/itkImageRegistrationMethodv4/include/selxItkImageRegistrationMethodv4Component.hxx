#include "selxItkImageRegistrationMethodv4Component.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageRegistrationMethodv4Component()
{
  m_theItkFilter = TheItkFilterType::New();
}
template<int Dimensionality, class TPixel>
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::~ItkImageRegistrationMethodv4Component()
{
}

// TODO: see if itkImageSourceInterface is the right way to connect itk filters..
/*
int ItkImageRegistrationMethodv4Component::Set(itkProcessObjectInterface* component)
{
  auto other = component->GetItkProcessObject();
  // connect the itk pipeline
  //this->m_theItkFilter->SetInput(other->GetOutputs()[0]);
  return 0;
}

itk::ProcessObject::Pointer ItkImageRegistrationMethodv4Component::GetItkProcessObject()
{
  return m_theItkFilter;
}

int ItkImageRegistrationMethodv4Component::Set(itkImageToImageFilterInterface* component)
{
  auto other = component->GetItkImageToImageFilter();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput(other->GetOutput());
  return 0;
}

itk::ImageToImageFilter<itk::Image<double, 3>, itk::Image<double, 3>>::Pointer ItkImageRegistrationMethodv4Component::GetItkImageToImageFilter()
{
  return m_theItkFilter;
}
*/
template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSource();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput(other->GetOutput());
  return 0;
}

//ItkImageSourceType::Pointer 
template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageSourcePointer ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::GetItkImageSource()
{
  return m_theItkFilter;
}
template<int Dimensionality, class TPixel>
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
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
  else if (criterion.first == "Sigma") //Supports this?
  {
    if (criterion.second.size() != 1)
    {
      meetsCriteria = false;
      //itkExceptionMacro("The criterion Sigma may have only 1 value");
    }
    else
    {
      auto const & criterionValue = *criterion.second.begin();
      try
      {
        this->m_theItkFilter->SetSigma(std::stod(criterionValue));
        meetsCriteria = true;
      }
      catch (itk::ExceptionObject & err)
      {
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  return meetsCriteria;
}

} //end namespace selx
