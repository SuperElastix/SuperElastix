#ifndef selxElastixComponent_h
#define selxElastixComponent_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "itkImageSource.h"

#include "elxElastixFilter.h"
#include "elxParameterObject.h"
#include "elxTransformixFilter.h"

#include <string.h>
#include "elxMacro.h"
namespace selx
{
  template <int Dimensionality, class TPixel>
  class ElastixComponent : 
    public Implements<
      Accepting< 
        itkImageSourceFixedInterface<Dimensionality, TPixel>,
        itkImageSourceMovingInterface<Dimensionality, TPixel>
      >,
      Providing< 
        GetItkImageInterface<Dimensionality, TPixel>, //Since elastixFilter is not a true itkfilter we cannot use itkImageSourceInterface (yet)
        RunRegistrationInterface
      >
    >
  {
  public:
    elxNewMacro(ElastixComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ElastixComponent();
    virtual ~ElastixComponent();
    
    typedef typename ComponentBase::CriterionType CriterionType;
    typedef TPixel PixelType;

    // the in and output image type of the component are chosen to be the same 
    typedef itk::Image<PixelType, Dimensionality> ConnectionImageType;

    // fixed and moving image types are all the same, these aliases can be used to be explicit. 
    typedef itk::Image<PixelType, Dimensionality> FixedImageType;
    typedef itk::Image<PixelType, Dimensionality> MovingImageType;

    typedef itk::ImageSource<ConnectionImageType> ItkImageSourceType;
    typedef typename ItkImageSourceType::Pointer ItkImageSourcePointer;

    typedef typename ConnectionImageType::Pointer ItkImagePointer;

    typedef elastix::ElastixFilter< FixedImageType, MovingImageType > TheItkFilterType;
    typedef elastix::ParameterObject elxParameterObjectType;
    typedef elxParameterObjectType::Pointer elxParameterObjectPointer;
    
    typedef elastix::TransformixFilter<FixedImageType> TransformixFilterType;


    typedef itk::ResampleImageFilter<MovingImageType, ConnectionImageType> ResampleFilterType;

    virtual int Set(itkImageSourceFixedInterface<Dimensionality, TPixel>*) override;
    virtual int Set(itkImageSourceMovingInterface<Dimensionality, TPixel>*) override;
    
    //Since elastixFilter is not a true itkfilter we cannot use itkImageSourceInterface (yet)
    //virtual ItkImageSourcePointer GetItkImageSource() override;
    virtual ItkImagePointer GetItkImage() override;
    virtual void RunRegistration() override;

    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    static const char * GetDescription() { return "Elastix Component"; };
  private:
    typename TheItkFilterType::Pointer m_theItkFilter;
    //elxParameterObjectPointer m_ParameterObject;

    typename ItkImagePointer m_OutputImage;
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
  class ElastixComponent < Dimensionality, double >
  {
    static inline const std::string GetPixelTypeNameString();
  };

  template <int Dimensionality>
  inline const std::string
    ElastixComponent<Dimensionality, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  */

  template <>
  inline const std::string
    ElastixComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ElastixComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ElastixComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ElastixComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
  template <>
  inline const std::string
    ElastixComponent<2, float>
    ::GetTypeNameString()
  {
    return std::string("2_float");
  }

  template <>
  inline const std::string
    ElastixComponent<2, double>
    ::GetTypeNameString()
  {
    return std::string("2_double");
  }

  template <>
  inline const std::string
    ElastixComponent<3,float>
    ::GetTypeNameString()
  {
    return std::string("3_float");
  }
  
  template <>
  inline const std::string
    ElastixComponent<3,double>
    ::GetTypeNameString()
  {
    return std::string("3_double");
  }
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxElastixComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h