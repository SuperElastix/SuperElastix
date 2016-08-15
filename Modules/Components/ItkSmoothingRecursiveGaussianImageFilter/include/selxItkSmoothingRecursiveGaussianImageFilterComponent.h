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

#ifndef selxItkSmoothingRecursiveGaussianImageFilterComponent_h
#define selxItkSmoothingRecursiveGaussianImageFilterComponent_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkImageSource.h"
#include <string.h>
#include "selxMacro.h"
namespace selx
{
  template <int Dimensionality, class TPixel>
  class ItkSmoothingRecursiveGaussianImageFilterComponent : 
    public SuperElastixComponent<
    Accepting< itkImageInterface<Dimensionality, TPixel> >,
    Providing< itkImageInterface<Dimensionality, TPixel> >
    >
  {
  public:
    selxNewMacro(ItkSmoothingRecursiveGaussianImageFilterComponent, ComponentBase);

    // itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkSmoothingRecursiveGaussianImageFilterComponent();
    virtual ~ItkSmoothingRecursiveGaussianImageFilterComponent();

    typedef TPixel PixelType;
    typedef itk::SmoothingRecursiveGaussianImageFilter<itk::Image<PixelType, Dimensionality>, itk::Image<PixelType, Dimensionality>> TheItkFilterType;
    typedef itk::Image<PixelType, Dimensionality> ItkImageType;
    typedef typename ItkImageType::Pointer ItkImagePointer;

    virtual int Set(itkImageInterface<Dimensionality, TPixel>*) override;
    virtual ItkImagePointer GetItkImage() override;

    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;    
    static const char * GetDescription() { return "ItkSmoothingRecursiveGaussianImageFilter Component"; };
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

  // unfortunately partial specialization of member functions is not allowed, without partially specializing the entire class.

  /*
  template <int Dimensionality>
  class ItkSmoothingRecursiveGaussianImageFilterComponent < Dimensionality, double >
  {
    static inline const std::string GetPixelTypeNameString();
  };

  template <int Dimensionality>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<Dimensionality, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  */

  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>
    ::GetTypeNameString()
  {
    return std::string("2_float");
  }

  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<2, double>
    ::GetTypeNameString()
  {
    return std::string("2_double");
  }

  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<3,float>
    ::GetTypeNameString()
  {
    return std::string("3_float");
  }
  
  template <>
  inline const std::string
    ItkSmoothingRecursiveGaussianImageFilterComponent<3,double>
    ::GetTypeNameString()
  {
    return std::string("3_double");
  }
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h