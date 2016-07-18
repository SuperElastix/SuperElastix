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

#ifndef selxItkImageRegistrationMethodv4Component_h
#define selxItkImageRegistrationMethodv4Component_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "itkImageRegistrationMethodv4.h"
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
  class ItkImageRegistrationMethodv4Component : 
    public Implements<
    Accepting< itkImageFixedInterface<Dimensionality, TPixel>, 
               itkImageMovingInterface<Dimensionality, TPixel>,
               itkMetricv4Interface<Dimensionality, TPixel>,
               itkOptimizerv4Interface<double>
             >,
    Providing< itkImageInterface<Dimensionality, TPixel>,
               DisplacementFieldItkImageSourceInterface<Dimensionality, TPixel>,
               RunRegistrationInterface
             >
    >
  {
  public:
    selxNewMacro(ItkImageRegistrationMethodv4Component, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageRegistrationMethodv4Component();
    virtual ~ItkImageRegistrationMethodv4Component();

    typedef TPixel PixelType;
    typedef itkOptimizerv4Interface<double>::InternalComputationValueType InternalComputationValueType;
    // the in and output image type of the component are chosen to be the same 
    typedef itk::Image<PixelType, Dimensionality> ConnectionImageType; 

    // fixed and moving image types are all the same, these aliases can be used to be explicit. 
    typedef typename itkImageFixedInterface<Dimensionality, TPixel>::ItkImageType FixedImageType;
    typedef typename itkImageMovingInterface<Dimensionality, TPixel>::ItkImageType MovingImageType;
    
    typedef typename itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality> DisplacementFieldImageType;
    typedef typename itkImageInterface<Dimensionality, TPixel>::ItkImageType ResultItkImageType;
    

    // TODO for now we hard code the transform to be a stationary velocity field. See Set(*MetricInterface) for implementation
    typedef double RealType;
    typedef itk::GaussianExponentialDiffeomorphicTransform<RealType, Dimensionality> ConstantVelocityFieldTransformType;
    typedef itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, ConstantVelocityFieldTransformType> TheItkFilterType;
    typedef typename TheItkFilterType::ImageMetricType ImageMetricType;
    typedef itk::RegistrationParameterScalesFromPhysicalShift<ImageMetricType> ScalesEstimatorType;
    typedef itk::ResampleImageFilter<MovingImageType, ConnectionImageType> ResampleFilterType;
    typedef itk::TransformToDisplacementFieldFilter<DisplacementFieldImageType> DisplacementFieldFilterType;
    
    //Accepting Interfaces:
    virtual int Set(itkImageFixedInterface<Dimensionality, TPixel>*) override;
    virtual int Set(itkImageMovingInterface<Dimensionality, TPixel>*) override;
    virtual int Set(itkMetricv4Interface<Dimensionality, TPixel>*) override;
    virtual int Set(itkOptimizerv4Interface<InternalComputationValueType>*) override;
    
    //Providing Interfaces:
    virtual typename ResultItkImageType::Pointer GetItkImage() override;
    virtual typename DisplacementFieldImageType::Pointer GetDisplacementFieldItkImage() override;

    virtual void RunRegistration() override;

    //BaseClass methods
    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;    
    //static const char * GetName() { return "ItkImageRegistrationMethodv4"; } ;
    static const char * GetDescription() { return "ItkImageRegistrationMethodv4 Component"; };
  private:
    typename TheItkFilterType::Pointer m_theItkFilter;
    typename ResampleFilterType::Pointer m_resampler;
    typename DisplacementFieldFilterType::Pointer m_DisplacementFieldFilter;
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

  // unfortunately partial specialization of member functions is not allowed, without partially specializing the entire class.

  /*
  template <int Dimensionality>
  class ItkImageRegistrationMethodv4Component < Dimensionality, double >
  {
    static inline const std::string GetPixelTypeNameString();
  };

  template <int Dimensionality>
  inline const std::string
    ItkImageRegistrationMethodv4Component<Dimensionality, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  */

  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<2, float>
    ::GetTypeNameString()
  {
    return std::string("2_float");
  }

  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<2, double>
    ::GetTypeNameString()
  {
    return std::string("2_double");
  }

  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<3,float>
    ::GetTypeNameString()
  {
    return std::string("3_float");
  }
  
  template <>
  inline const std::string
    ItkImageRegistrationMethodv4Component<3,double>
    ::GetTypeNameString()
  {
    return std::string("3_double");
  }
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageRegistrationMethodv4Component.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h