#ifndef selxItkImageFilterSink_h
#define selxItkImageFilterSink_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "selxMacro.h"
#include "itkImageFileWriter.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  class ItkImageFilterSinkComponent :
    public Implements <
    Accepting< itkImageSourceInterface<Dimensionality, TPixel> >,
    Providing < SinkInterface >
    >
  {
  public:
    selxNewMacro(ItkImageFilterSinkComponent, ComponentBase);

    //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    ItkImageFilterSinkComponent();
    virtual ~ItkImageFilterSinkComponent();

    typedef itk::ImageSource<itk::Image<TPixel, Dimensionality>> ItkImageSourceType;

    virtual int Set(itkImageSourceInterface<Dimensionality, TPixel>*) override;
    virtual bool ConnectToOverlordSink(itk::Object::Pointer) override;

    virtual bool MeetsCriterion(const ComponentBase::CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageFilterSink Component"; };
  private:
    itk::ProcessObject::Pointer m_Sink;
    typename itk::ImageFileWriter<itk::Image<TPixel, Dimensionality>>::Pointer m_SinkWriter;
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
    ItkImageFilterSinkComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    ItkImageFilterSinkComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    ItkImageFilterSinkComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    ItkImageFilterSinkComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkImageFilterSink.hxx"
#endif
#endif // #define selxItkImageFilterSink_h