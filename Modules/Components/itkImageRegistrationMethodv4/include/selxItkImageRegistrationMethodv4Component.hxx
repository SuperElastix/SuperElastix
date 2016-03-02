#include "selxItkImageRegistrationMethodv4Component.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageRegistrationMethodv4Component()
{
  m_theItkFilter = TheItkFilterType::New();
  m_resampler = ResampleFilterType::New();
  m_DisplacementFieldFilter = DisplacementFieldFilterType::New();
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template<int Dimensionality, class TPixel>
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::~ItkImageRegistrationMethodv4Component()
{
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceFixedInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSourceFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage(other->GetOutput());
  return 1;
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkImageSourceMovingInterface<Dimensionality, TPixel>* component)
{
  auto other = component->GetItkImageSourceMoving();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage(other->GetOutput());
  return 1;
}
template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkMetricv4Interface<Dimensionality, TPixel>* component)
{
  this->m_theItkFilter->SetMetric(component->GetItkMetricv4());

  return 1;
}

template<int Dimensionality, class TPixel>
void ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::RunRegistration(void)
{

  FixedImageType::ConstPointer fixedImage = this->m_theItkFilter->GetFixedImage();
  MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  // Below some hard coded options. Eventually, these should be part of new components.

  //TODO: Setting the optimizer explicitly is a work around for a bug in itkv4. 
  //TODO: report bug to itk.
  typedef itk::GradientDescentOptimizerv4       OptimizerType;
  OptimizerType::Pointer      optimizer = OptimizerType::New();
  this->m_theItkFilter->SetOptimizer(optimizer);

  // TODO: for now we hard code the transform to be a stationary velocity field. See template declaration.

  //typedef itk::CompositeTransform<RealType, Dimensionality> CompositeTransformType;
  //typename CompositeTransformType::Pointer compositeTransform = CompositeTransformType::New();
  
  //typedef itk::IdentityTransform < RealType, Dimensionality> IdentityTransformType;
  //typename IdentityTransformType::Pointer idTransform = IdentityTransformType::New();
  //compositeTransform->AddTransform(idTransform);
  

  typedef itk::Vector<RealType, Dimensionality> VectorType;
  VectorType zeroVector(0.0);
  typedef itk::Image<VectorType, Dimensionality> DisplacementFieldType;
  typedef itk::Image<VectorType, Dimensionality> ConstantVelocityFieldType;
  typename ConstantVelocityFieldType::Pointer displacementField = ConstantVelocityFieldType::New();
  displacementField->CopyInformation(fixedImage);
  displacementField->SetRegions(fixedImage->GetBufferedRegion());
  displacementField->Allocate();
  displacementField->FillBuffer(zeroVector);

  typename ConstantVelocityFieldTransformType::Pointer fieldTransform = ConstantVelocityFieldTransformType::New();
  fieldTransform->SetGaussianSmoothingVarianceForTheUpdateField(0.75);
  fieldTransform->SetGaussianSmoothingVarianceForTheConstantVelocityField(1.5);
  fieldTransform->SetConstantVelocityField(displacementField);
  fieldTransform->SetCalculateNumberOfIntegrationStepsAutomatically(true);
  fieldTransform->IntegrateVelocityField();

  //this->m_theItkFilter->SetMovingInitialTransform(compositeTransform);
  //this->m_theItkFilter->SetMovingInitialTransform(idTransform);
  
  this->m_theItkFilter->SetNumberOfLevels(1);
  /*
  // Shrink the virtual domain by specified factors for each level.  See documentation
  // for the itkShrinkImageFilter for more detailed behavior.
  typename TheItkFilterType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize(3);
  shrinkFactorsPerLevel[0] = 3;
  shrinkFactorsPerLevel[1] = 2;
  shrinkFactorsPerLevel[2] = 1;
  this->m_theItkFilter->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

  // Smooth by specified gaussian sigmas for each level.  These values are specified in
  // physical units.
  typename TheItkFilterType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize(3);
  smoothingSigmasPerLevel[0] = 2;
  smoothingSigmasPerLevel[1] = 1;
  smoothingSigmasPerLevel[2] = 1;
  this->m_theItkFilter->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);

  typedef itk::GaussianExponentialDiffeomorphicTransformParametersAdaptor<ConstantVelocityFieldTransformType> VelocityFieldTransformAdaptorType;

  typename TheItkFilterType::TransformParametersAdaptorsContainerType adaptors;
  
  for (unsigned int level = 0; level < shrinkFactorsPerLevel.Size(); level++)
  {
    // We use the shrink image filter to calculate the fixed parameters of the virtual
    // domain at each level.  To speed up calculation and avoid unnecessary memory
    // usage, we could calculate these fixed parameters directly.

    typedef itk::ShrinkImageFilter<ConstantVelocityFieldType, ConstantVelocityFieldType> ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
    shrinkFilter->SetShrinkFactors(shrinkFactorsPerLevel[level]);
    shrinkFilter->SetInput(fieldTransform->GetConstantVelocityField());

    typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
    fieldTransformAdaptor->SetRequiredSpacing(shrinkFilter->GetOutput()->GetSpacing());
    fieldTransformAdaptor->SetRequiredSize(shrinkFilter->GetOutput()->GetBufferedRegion().GetSize());
    fieldTransformAdaptor->SetRequiredDirection(shrinkFilter->GetOutput()->GetDirection());
    fieldTransformAdaptor->SetRequiredOrigin(shrinkFilter->GetOutput()->GetOrigin());

    adaptors.push_back(fieldTransformAdaptor.GetPointer());
  }
  */
  /*
  typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
  fieldTransformAdaptor->SetRequiredSpacing(fixedImage->GetSpacing());
  fieldTransformAdaptor->SetRequiredSize(fixedImage->GetBufferedRegion().GetSize());
  fieldTransformAdaptor->SetRequiredDirection(fixedImage->GetDirection());
  fieldTransformAdaptor->SetRequiredOrigin(fixedImage->GetOrigin());

  adaptors.push_back(fieldTransformAdaptor.GetPointer());
  
  this->m_theItkFilter->SetTransformParametersAdaptorsPerLevel(adaptors);
  */
  this->m_theItkFilter->SetInitialTransform(fieldTransform);
  this->m_theItkFilter->InPlaceOn();
  
  
  // 
  this->m_theItkFilter->Update();

  //this->m_resampler->SetTransform(this->m_theItkFilter->GetTransform());
  this->m_resampler->SetTransform(this->m_theItkFilter->GetTransformOutput()->Get());
  //this->m_resampler->SetTransform(this->m_theItkFilter->GetOutput());
  this->m_resampler->SetInput(movingImage);
  this->m_resampler->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  this->m_resampler->SetOutputOrigin(fixedImage->GetOrigin());
  this->m_resampler->SetOutputSpacing(fixedImage->GetSpacing());
  this->m_resampler->SetOutputDirection(fixedImage->GetDirection());
  this->m_resampler->SetDefaultPixelValue(0);

  //this->m_DisplacementFieldFilter->SetTransformInput(this->m_theItkFilter->GetTransformOutput());
  //this->m_DisplacementFieldFilter->SetTransformInput(this->m_theItkFilter->GetTransformOutput()->Get());
  this->m_DisplacementFieldFilter->SetTransform(this->m_theItkFilter->GetTransformOutput()->Get());
  this->m_DisplacementFieldFilter->SetSize(fixedImage->GetBufferedRegion().GetSize()); //should be virtual image...
  this->m_DisplacementFieldFilter->SetOutputOrigin(fixedImage->GetOrigin());
  this->m_DisplacementFieldFilter->SetOutputSpacing(fixedImage->GetSpacing());
  this->m_DisplacementFieldFilter->SetOutputDirection(fixedImage->GetDirection());

  this->m_DisplacementFieldFilter->SetReferenceImage(fixedImage);
}

template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageSourcePointer ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::GetItkImageSource()
{
  
  return (ItkImageSourcePointer) this->m_resampler;
}


template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::DisplacementFieldItkImageSourcePointer ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::GetDisplacementFieldItkImageSource()
{

  return (DisplacementFieldItkImageSourcePointer) this->m_DisplacementFieldFilter;
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
  return meetsCriteria;
}

} //end namespace selx
