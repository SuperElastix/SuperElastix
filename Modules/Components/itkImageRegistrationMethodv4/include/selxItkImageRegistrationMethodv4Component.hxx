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

#include "selxItkImageRegistrationMethodv4Component.h"

//TODO: get rid of these
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageFileWriter.h"

namespace selx
{
  template<typename TFilter>
  class CommandIterationUpdate : public itk::Command
  {
  public:
    typedef CommandIterationUpdate   Self;
    typedef itk::Command             Superclass;
    typedef itk::SmartPointer<Self>  Pointer;
    itkNewMacro(Self);

    typedef itk::GradientDescentOptimizerv4    OptimizerType;
    typedef   const OptimizerType *   OptimizerPointer;

  protected:
    CommandIterationUpdate() {};

  public:

    virtual void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
    {
      Execute((const itk::Object *) caller, event);
    }

      virtual void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
    {
      const TFilter * filter = static_cast< const TFilter * >(object);
      if (typeid(event) == typeid(itk::MultiResolutionIterationEvent))
      {

        unsigned int currentLevel = filter->GetCurrentLevel();
        typename TFilter::ShrinkFactorsPerDimensionContainerType shrinkFactors = filter->GetShrinkFactorsPerDimension(currentLevel);
        typename TFilter::SmoothingSigmasArrayType smoothingSigmas = filter->GetSmoothingSigmasPerLevel();
        typename TFilter::TransformParametersAdaptorsContainerType adaptors = filter->GetTransformParametersAdaptorsPerLevel();

        const itk::ObjectToObjectOptimizerBase * optimizerBase = filter->GetOptimizer();
        typedef itk::GradientDescentOptimizerv4 GradientDescentOptimizerv4Type;
        typename GradientDescentOptimizerv4Type::ConstPointer optimizer = dynamic_cast<const GradientDescentOptimizerv4Type *>(optimizerBase);
        if (!optimizer)
        {
          itkGenericExceptionMacro("Error dynamic_cast failed");
        }
        typename GradientDescentOptimizerv4Type::DerivativeType gradient = optimizer->GetGradient();

        /* orig
        std::cout << "  Current level = " << currentLevel << std::endl;
        std::cout << "    shrink factor = " << shrinkFactors[currentLevel] << std::endl;
        std::cout << "    smoothing sigma = " << smoothingSigmas[currentLevel] << std::endl;
        std::cout << "    required fixed parameters = " << adaptors[currentLevel]->GetRequiredFixedParameters() << std::endl;
        */

        //debug:
        std::cout << "  CL Current level:           " << currentLevel << std::endl;
        std::cout << "   SF Shrink factor:          " << shrinkFactors << std::endl;
        std::cout << "   SS Smoothing sigma:        " << smoothingSigmas[currentLevel] << std::endl;
        std::cout << "   RFP Required fixed params: " << adaptors[currentLevel]->GetRequiredFixedParameters() << std::endl;
        std::cout << "   LR Final learning rate:    " << optimizer->GetLearningRate() << std::endl;
        std::cout << "   FM Final metric value:     " << optimizer->GetCurrentMetricValue() << std::endl;
        std::cout << "   SC Optimizer scales:       " << optimizer->GetScales() << std::endl;
        std::cout << "   FG Final metric gradient (sample of values): ";
        if (gradient.GetSize() < 10)
        {
          std::cout << gradient;
        }
        else
        {
          for (itk::SizeValueType i = 0; i < gradient.GetSize(); i += (gradient.GetSize() / 16))
          {
            std::cout << gradient[i] << " ";
          }
        }
        std::cout << std::endl;

      }
      else if (!(itk::IterationEvent().CheckEvent(&event)))
      {
        return;
      }
      else
      {
        OptimizerPointer optimizer = static_cast<OptimizerPointer>(object);
        std::cout << optimizer->GetCurrentIteration() << ": " ;
        std::cout << optimizer->GetCurrentMetricValue() << std::endl;
        //std::cout << optimizer->GetInfinityNormOfProjectedGradient() << std::endl;
      }
    }


  };



  template<int Dimensionality, class TPixel>
  ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ItkImageRegistrationMethodv4Component()
{
  m_theItkFilter = TheItkFilterType::New();
  m_resampler = ResampleFilterType::New();
  //m_DisplacementFieldFilter = DisplacementFieldFilterType::New();
  //m_DisplacementFieldFilter->GetTransformInput()->Graft<ConstantVelocityFieldTransformType>(&(const_cast<ConstantVelocityFieldTransformType>( m_theItkFilter->GetOutput())));
  //m_DisplacementFieldFilter->GetTransformInput()->Graft(m_theItkFilter->GetOutput());
  
  typename ConstantVelocityFieldTransformType::Pointer transform = ConstantVelocityFieldTransformType::New();
  m_theItkFilter->SetInitialTransform(transform);

  m_theItkFilter->InPlaceOff();

  typename itk::DataObjectDecorator<typename ConstantVelocityFieldTransformType::Superclass::Superclass::Superclass>::Pointer decoratedDummyTransform = itk::DataObjectDecorator<typename ConstantVelocityFieldTransformType::Superclass::Superclass::Superclass>::New();
  typename ConstantVelocityFieldTransformType::Pointer dummyTranform = ConstantVelocityFieldTransformType::New();
  decoratedDummyTransform->Set(dummyTranform);

  //decoratedTransform->Set(m_theItkFilter->GetOutput()->Get());
  //m_DisplacementFieldFilter->SetTransformInput(const_cast< itk::DataObjectDecorator<ConstantVelocityFieldTransformType::Superclass::Superclass::Superclass>*>(decoratedTransform));
  
  //m_DisplacementFieldFilter->SetTransformInput(decoratedDummyTransform);
  //m_theItkFilter->GetOutput()->Graft(m_DisplacementFieldFilter->GetTransformInput());
  //m_DisplacementFieldFilter->GetTransformInput()->Graft(decoratedTransform);

  //m_DisplacementFieldFilter->SetTransformInput(const_cast< itk::DataObjectDecorator<ConstantVelocityFieldTransformType::Superclass::Superclass::Superclass>*>(m_theItkFilter->GetOutput()));
  //m_DisplacementFieldFilter->GetTransformInput()->Graft(const_cast< itk::DataObjectDecorator<ConstantVelocityFieldTransformType>*>(m_theItkFilter->GetOutput()));
   //m_DisplacementFieldFilter->GetOutput()->SetLargestPossibleRegion()
  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}

template<int Dimensionality, class TPixel>
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::~ItkImageRegistrationMethodv4Component()
{
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
::Set(itkImageFixedInterface<Dimensionality, TPixel>* component)
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_theItkFilter->SetFixedImage(fixedImage);

  //this->m_resampler->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  this->m_resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());  //should be virtual image...
  this->m_resampler->SetOutputOrigin(fixedImage->GetOrigin());
  this->m_resampler->SetOutputSpacing(fixedImage->GetSpacing());
  this->m_resampler->SetOutputDirection(fixedImage->GetDirection());
  this->m_resampler->SetDefaultPixelValue(0);

  return 0;
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
::Set(itkImageMovingInterface<Dimensionality, TPixel>* component)
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_theItkFilter->SetMovingImage(movingImage);

  this->m_resampler->SetInput(movingImage);
  this->m_resampler->UpdateOutputInformation();
  return 0;
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkMetricv4Interface<Dimensionality, TPixel>* component)
{
   this->m_theItkFilter->SetMetric(component->GetItkMetricv4());

  return 1;
}

template<int Dimensionality, class TPixel>
int ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::Set(itkOptimizerv4Interface<InternalComputationValueType>* component)
{
  this->m_theItkFilter->SetOptimizer(component->GetItkOptimizerv4());

  return 1;
}

template<int Dimensionality, class TPixel>
void ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::RunRegistration(void)
{

  typename FixedImageType::ConstPointer fixedImage = this->m_theItkFilter->GetFixedImage();
  typename MovingImageType::ConstPointer movingImage = this->m_theItkFilter->GetMovingImage();

  // Below some hard coded options. Eventually, these should be part of new components.

  //TODO: Setting the optimizer explicitly is a work around for a bug in itkv4. 
  //TODO: report bug to itk: when setting a metric, the optimizer must be set explicitly as well, since default optimizer setup breaks.
  //typedef itk::GradientDescentOptimizerv4       OptimizerType;
  //OptimizerType::Pointer      optimizer = OptimizerType::New();
  //optimizer->SetNumberOfIterations(100);
  //optimizer->SetLearningRate(1.0);
  
  typename ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
    
  typedef itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType> MSDMetricType;
  typename MSDMetricType::Pointer msdMetric = dynamic_cast<MSDMetricType*>(this->m_theItkFilter->GetModifiableMetric());

  typedef itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<FixedImageType, MovingImageType> ANTSCCMetricType;
  typename ANTSCCMetricType::Pointer nccMetric = dynamic_cast<ANTSCCMetricType*>(this->m_theItkFilter->GetModifiableMetric());

  ImageMetricType* theMetric = dynamic_cast<ImageMetricType*>(this->m_theItkFilter->GetModifiableMetric());;

  auto optimizer = dynamic_cast<itk::GradientDescentOptimizerv4 *>(this->m_theItkFilter->GetModifiableOptimizer());
  //auto optimizer = dynamic_cast<itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > *>(this->m_theItkFilter->GetModifiableOptimizer());
  

  if (msdMetric)
  {
    //TODO: get rid of component specific checking
    scalesEstimator->SetMetric(msdMetric);
    optimizer->SetLearningRate(0.001);
  }
  else if (nccMetric)
  {
    //TODO: get rid of component specific checking
    scalesEstimator->SetMetric(nccMetric);
    optimizer->SetLearningRate(100.0);
  }
  else if (theMetric)
  {
    scalesEstimator->SetMetric(theMetric);
  }
  else
  {
    itkExceptionMacro("Error casting to either MeanSquaresImageToImageMetricv4 or ANTSNeighborhoodCorrelationImageToImageMetricv4 or ImageMetricType failed");
  }
 

  //std::cout << "estimated step scale: " << scalesEstimator->EstimateStepScale(1.0);
  scalesEstimator->SetTransformForward(true);
  scalesEstimator->SetSmallParameterVariation(1.0);
  

  //this->m_theItkFilter->GetModifyableMetric();

  optimizer->SetScalesEstimator(ITK_NULLPTR);
  //optimizer->SetScalesEstimator(scalesEstimator);
  optimizer->SetDoEstimateLearningRateOnce(false); //true by default
  optimizer->SetDoEstimateLearningRateAtEachIteration(false);


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
  //fieldTransform->SetGaussianSmoothingVarianceForTheUpdateField(0.75);
  //fieldTransform->SetGaussianSmoothingVarianceForTheConstantVelocityField(1.5);
  fieldTransform->SetGaussianSmoothingVarianceForTheUpdateField(3.0);
  fieldTransform->SetGaussianSmoothingVarianceForTheConstantVelocityField(6.0);
  fieldTransform->SetConstantVelocityField(displacementField);
  fieldTransform->SetCalculateNumberOfIntegrationStepsAutomatically(true);
  fieldTransform->IntegrateVelocityField();

  //this->m_theItkFilter->SetMovingInitialTransform(compositeTransform);
  //this->m_theItkFilter->SetMovingInitialTransform(idTransform);
  
  this->m_theItkFilter->SetNumberOfLevels(3);
  
  // Shrink the virtual domain by specified factors for each level.  See documentation
  // for the itkShrinkImageFilter for more detailed behavior.
  typename TheItkFilterType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize(3);
  shrinkFactorsPerLevel[0] = 4;
  shrinkFactorsPerLevel[1] = 2;
  shrinkFactorsPerLevel[2] = 1;
  this->m_theItkFilter->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

  // Smooth by specified gaussian sigmas for each level.  These values are specified in
  // physical units.
  typename TheItkFilterType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize(3);
  smoothingSigmasPerLevel[0] = 4;
  smoothingSigmasPerLevel[1] = 2;
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
    shrinkFilter->Update();

    typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
    fieldTransformAdaptor->SetRequiredSpacing(shrinkFilter->GetOutput()->GetSpacing());
    fieldTransformAdaptor->SetRequiredSize(shrinkFilter->GetOutput()->GetBufferedRegion().GetSize());
    fieldTransformAdaptor->SetRequiredDirection(shrinkFilter->GetOutput()->GetDirection());
    fieldTransformAdaptor->SetRequiredOrigin(shrinkFilter->GetOutput()->GetOrigin());

    adaptors.push_back(fieldTransformAdaptor.GetPointer());
  }
  
  /*
  typename VelocityFieldTransformAdaptorType::Pointer fieldTransformAdaptor = VelocityFieldTransformAdaptorType::New();
  fieldTransformAdaptor->SetRequiredSpacing(fixedImage->GetSpacing());
  fieldTransformAdaptor->SetRequiredSize(fixedImage->GetBufferedRegion().GetSize());
  fieldTransformAdaptor->SetRequiredDirection(fixedImage->GetDirection());
  fieldTransformAdaptor->SetRequiredOrigin(fixedImage->GetOrigin());

  adaptors.push_back(fieldTransformAdaptor.GetPointer());
  */
  this->m_theItkFilter->SetTransformParametersAdaptorsPerLevel(adaptors);
  
  this->m_theItkFilter->SetInitialTransform(fieldTransform);
  this->m_theItkFilter->InPlaceOn();
  
  
  typedef CommandIterationUpdate<TheItkFilterType> DisplacementFieldRegistrationCommandType;
  typename DisplacementFieldRegistrationCommandType::Pointer displacementFieldObserver = DisplacementFieldRegistrationCommandType::New();
  this->m_theItkFilter->AddObserver(itk::IterationEvent(), displacementFieldObserver);
  
  // perform the actual registration
  this->m_theItkFilter->Update();

  // TODO get access to the inverse transform
  //ConstantVelocityFieldTransformType::ConstPointer tranform = this->m_theItkFilter->GetTransformOutput()->Get();
  //ConstantVelocityFieldTransformType::ConstPointer inversetranform = tranform->GetInverseTransform();

  //ConstantVelocityFieldTransformType::ConstPointer tranform = this->m_theItkFilter->GetTransformOutput()->Get();
  //ConstantVelocityFieldTransformType::Pointer inversetranform = ConstantVelocityFieldTransformType::New();
  //typename ConstantVelocityFieldTransformType::Superclass::Pointer inversetranform = fieldTransform->GetInverseTransform();
  
  //Temporary solution: create new displacement field transforms
  typedef itk::DisplacementFieldTransform<RealType, Dimensionality> DisplacementFieldTransformType;
  typename DisplacementFieldTransformType::Pointer forwardDisplacement = DisplacementFieldTransformType::New();
  forwardDisplacement->SetDisplacementField(fieldTransform->GetDisplacementField());
  //forwardDisplacement->SetSize(fixedImage->GetBufferedRegion().GetSize());  //should be virtual image...
  //forwardDisplacement->SetOutputOrigin(fixedImage->GetOrigin());
  //forwardDisplacement->SetOutputSpacing(fixedImage->GetSpacing());
  //forwardDisplacement->SetOutputDirection(fixedImage->GetDirection());

  typename DisplacementFieldTransformType::Pointer backwardDisplacement = DisplacementFieldTransformType::New();
  backwardDisplacement->SetDisplacementField(fieldTransform->GetDisplacementField());


  auto inversetranform = fieldTransform->GetInverseTransform();
  
  //auto inversetranform = tranform->GetInverseTransform();
  //fieldTransform->GetInverse(inversetranform);
  //inversetranform->IntegrateVelocityField();
  //inversetranform->IntegrateVelocityField();

  //this->m_resampler->SetTransform(this->m_theItkFilter->GetTransform());
  //this->m_resampler->SetTransform(this->m_theItkFilter->GetTransformOutput()->Get()->GetInverseTransform());
  //this->m_resampler->SetTransform(this->m_theItkFilter->GetTransformOutput()->Get());
  
  //BIG TODO: the resampler is insensitive for any of these options:
  //this->m_resampler->SetTransform(inversetranform);
  //this->m_resampler->SetTransform(fieldTransform);
  
  this->m_resampler->SetTransform(forwardDisplacement);
  
  //this->m_resampler->SetTransform(this->m_theItkFilter->GetOutput());

  
  // TODO: is this needed?
  //this->m_resampler->Update();
  //this->m_DisplacementFieldFilter->SetTransformInput(this->m_theItkFilter->GetTransformOutput());
  //this->m_DisplacementFieldFilter->SetTransformInput(this->m_theItkFilter->GetTransformOutput()->Get());
  
  //BIG TODO: the DisplacementFieldFilter is insensitive for any of these options:
  //this->m_DisplacementFieldFilter->SetTransform(inversetranform);
  //this->m_DisplacementFieldFilter->SetTransform(fieldTransform);

}

template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::ResultItkImageType::Pointer 
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
::GetItkImage()
{
  return this->m_resampler->GetOutput();
}


template<int Dimensionality, class TPixel>
typename ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>::TransformPointer
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
::GetItkTransform()
{

  //return this->m_theItkFilter->GetOutput().GetPointer();
  return this->m_theItkFilter->GetModifiableTransform();
}


template<int Dimensionality, class TPixel>
bool
ItkImageRegistrationMethodv4Component< Dimensionality, TPixel>
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
