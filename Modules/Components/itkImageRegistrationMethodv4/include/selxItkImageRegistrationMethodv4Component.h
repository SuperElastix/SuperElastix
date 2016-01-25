#ifndef selxItkImageRegistrationMethodv4Component_h
#define selxItkImageRegistrationMethodv4Component_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkImageSource.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  template <int Dimensionality, class TPixel>
  class ItkImageRegistrationMethodv4Component : 
    public Implements<
    Accepting< itkImageSourceInterface<Dimensionality, TPixel> >,
    Providing< itkImageSourceInterface<Dimensionality, TPixel> >
    >
    // TODO: see if itkImageSourceInterface is the right way to connect itk filters..
    //Accepting< itkProcessObjectInterface, itkImageToImageFilterInterface >,
    //Providing< itkProcessObjectInterface, itkImageToImageFilterInterface >
    //>
  {
  public:
    elxNewMacro(ItkImageRegistrationMethodv4Component, ComponentBase);

    itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageRegistrationMethodv4Component();
    virtual ~ItkImageRegistrationMethodv4Component();

    typedef TPixel PixelType;
    typedef itk::ImageRegistrationMethodv4<itk::Image<PixelType, Dimensionality>, itk::Image<PixelType, Dimensionality>> TheItkFilterType;
    typedef itk::ImageSource<itk::Image<PixelType, Dimensionality>> ItkImageSourceType;
    typedef typename ItkImageSourceType::Pointer ItkImageSourcePointer;

    // TODO: see if itkImageSourceInterface is the right way to connect itk filters..
    /*
    virtual int Set(itkProcessObjectInterface*) override;
    virtual itk::ProcessObject::Pointer GetItkProcessObject() override;

    virtual int Set(itkImageToImageFilterInterface*) override;
    virtual itk::ImageToImageFilter<itk::Image<double, 3>, itk::Image<double, 3>>::Pointer GetItkImageToImageFilter() override;
    */
    virtual int Set(itkImageSourceInterface<Dimensionality, TPixel>*) override;
    virtual ItkImageSourcePointer GetItkImageSource() override;

    //int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;    
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "ItkImageRegistrationMethodv4 Component"; };
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