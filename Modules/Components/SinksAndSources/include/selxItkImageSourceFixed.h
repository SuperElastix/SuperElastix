#ifndef selxItkImageSourceFixed_h
#define selxItkImageSourceFixed_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  template<int Dimensionality, class TPixel>
  class ItkImageSourceFixedComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface, itkImageSourceFixedInterface<Dimensionality, TPixel > >
    >
  {
  public:
    elxNewMacro(ItkImageSourceFixedComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageSourceFixedComponent();
    virtual ~ItkImageSourceFixedComponent();

    typedef typename itk::ImageSource<itk::Image<TPixel, Dimensionality>> ItkImageSourceFixedType;
    

    virtual typename ItkImageSourceFixedType::Pointer GetItkImageSourceFixed() override;
    virtual bool ConnectToOverlordSource(itk::Object::Pointer) override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageSourceFixed Component"; };
  private:
    typename ItkImageSourceFixedType::Pointer m_Source;

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
    ItkImageSourceFixedComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkImageSourceFixedComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkImageSourceFixedComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkImageSourceFixedComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageSourceFixed.hxx"
#endif
#endif // #define selxItkImageSourceFixed_h