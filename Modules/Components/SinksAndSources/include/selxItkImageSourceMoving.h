#ifndef selxItkImageSourceMoving_h
#define selxItkImageSourceMoving_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "selxMacro.h"
namespace selx
{
  template<int Dimensionality, class TPixel>
  class ItkImageSourceMovingComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface, itkImageSourceMovingInterface<Dimensionality, TPixel > >
    >
  {
  public:
    selxNewMacro(ItkImageSourceMovingComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageSourceMovingComponent();
    virtual ~ItkImageSourceMovingComponent();

    typedef itk::ImageSource<itk::Image<TPixel, Dimensionality>> ItkImageSourceMovingType;
    

    virtual typename ItkImageSourceMovingType::Pointer GetItkImageSourceMoving() override;
    virtual bool ConnectToOverlordSource(itk::Object::Pointer) override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageSourceMoving Component"; };
  private:
    typename ItkImageSourceMovingType::Pointer m_Source;
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
    ItkImageSourceMovingComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkImageSourceMovingComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkImageSourceMovingComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkImageSourceMovingComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageSourceMoving.hxx"
#endif
#endif // #define selxItkImageSourceMoving_h