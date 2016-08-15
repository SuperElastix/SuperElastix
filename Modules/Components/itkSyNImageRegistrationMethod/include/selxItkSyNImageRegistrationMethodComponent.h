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

#ifndef selxItkSyNImageRegistrationMethodComponent_h
#define selxItkSyNImageRegistrationMethodComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "itkSyNImageRegistrationMethod.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageSource.h"
#include <itkTransformToDisplacementFieldFilter.h>
#include <string.h>
#include "selxMacro.h"


#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"


namespace selx
{
  template <int Dimensionality, class TPixel>
  class ItkSyNImageRegistrationMethodComponent : 
    public SuperElastixComponent<
    Accepting< itkImageFixedInterface<Dimensionality, TPixel>, 
               itkImageMovingInterface<Dimensionality, TPixel>,
               itkMetricv4Interface<Dimensionality, TPixel>,
             >,
    Providing< itkTransformInterface<double, Dimensionality>,
               RunRegistrationInterface
             >
    >
  {
  public:
    selxNewMacro(ItkSyNImageRegistrationMethodComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkSyNImageRegistrationMethodComponent();
    virtual ~ItkSyNImageRegistrationMethodComponent();

    typedef TPixel PixelType;
    
    // Get the type definitions from the interfaces 
    typedef typename itkImageFixedInterface<Dimensionality, TPixel>::ItkImageType FixedImageType;
    typedef typename itkImageMovingInterface<Dimensionality, TPixel>::ItkImageType MovingImageType;
    typedef typename itkTransformInterface<double, Dimensionality>::TransformType TransformType;
    typedef typename itkTransformInterface<double, Dimensionality>::TransformPointer TransformPointer;

    typedef typename itkTransformInterface<double, Dimensionality>::InternalComputationValueType TransformInternalComputationValueType; //should be from class template
    
    typedef itk::SyNImageRegistrationMethod<FixedImageType, MovingImageType> TheItkFilterType;
    typedef typename TheItkFilterType::ImageMetricType ImageMetricType;
    typedef itk::RegistrationParameterScalesFromPhysicalShift<ImageMetricType> ScalesEstimatorType;
    
    //Accepting Interfaces:
    virtual int Set(itkImageFixedInterface<Dimensionality, TPixel>*) override;
    virtual int Set(itkImageMovingInterface<Dimensionality, TPixel>*) override;
    virtual int Set(itkMetricv4Interface<Dimensionality, TPixel>*) override;

    
    //Providing Interfaces:
    virtual TransformPointer GetItkTransform() override;
    virtual void RunRegistration() override;

    //BaseClass methods
    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;    
    //static const char * GetName() { return "ItkSyNImageRegistrationMethod"; } ;
    static const char * GetDescription() { return "ItkSyNImageRegistrationMethod Component"; };
  private:
    typename TheItkFilterType::Pointer m_theItkFilter;
  protected:
    /* The following struct returns the string name of computation type */
    /* default implementation */

    static inline const std::string GetTypeNameString()
    {
      itkGenericExceptionMacro(<< "Unknown ScalarType" << typeid(TPixel).name());
      // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
      // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
    }

    static inline const std::string GetPixelTypeNameString()
    {
      itkGenericExceptionMacro(<< "Unknown PixelType" << typeid(TPixel).name());
      // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
      // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
    }

  };

  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<2, float>
    ::GetTypeNameString()
  {
    return std::string("2_float");
  }

  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<2, double>
    ::GetTypeNameString()
  {
    return std::string("2_double");
  }

  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<3,float>
    ::GetTypeNameString()
  {
    return std::string("3_float");
  }
  
  template <>
  inline const std::string
    ItkSyNImageRegistrationMethodComponent<3,double>
    ::GetTypeNameString()
  {
    return std::string("3_double");
  }
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkSyNImageRegistrationMethodComponent.hxx"
#endif
#endif // #define selxItkSyNImageRegistrationMethodComponent_h