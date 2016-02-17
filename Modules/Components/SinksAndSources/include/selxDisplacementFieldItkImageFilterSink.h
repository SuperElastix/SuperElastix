#ifndef selxDisplacementFieldItkImageFilterSink_h
#define selxDisplacementFieldItkImageFilterSink_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "elxMacro.h"
#include "itkImageFileWriter.h"

namespace selx
{
  template<int Dimensionality, class TPixel>
  class DisplacementFieldItkImageFilterSinkComponent :
    public Implements <
    Accepting< DisplacementFieldItkImageSourceInterface<Dimensionality, TPixel> >,
    Providing < SinkInterface >
    >
  {
  public:
    elxNewMacro(DisplacementFieldItkImageFilterSinkComponent, ComponentBase);

    itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

    DisplacementFieldItkImageFilterSinkComponent();
    virtual ~DisplacementFieldItkImageFilterSinkComponent();

    typedef TPixel PixelType;

    typedef itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality> DeformationFieldImageType;
    typedef itk::ImageSource<DeformationFieldImageType> DeformationFieldItkImageSourceType;
    typedef typename DeformationFieldItkImageSourceType::Pointer DeformationFieldItkImageSourcePointer;

    virtual int Set(DisplacementFieldItkImageSourceInterface<Dimensionality, TPixel>*) override;
    virtual bool ConnectToOverlordSink(itk::Object::Pointer) override;

    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    static const char * GetDescription() { return "DisplacementFieldItkImageFilterSink Component"; };
  private:
    itk::ProcessObject::Pointer m_Sink;
    typename itk::ImageFileWriter<itk::Image<itk::Vector<TPixel,Dimensionality>, Dimensionality>>::Pointer m_SinkWriter;
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
    DisplacementFieldItkImageFilterSinkComponent<2, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }


  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<2, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<3, float>
    ::GetPixelTypeNameString()
  {
    return std::string("float");
  }

  template <>
  inline const std::string
    DisplacementFieldItkImageFilterSinkComponent<3, double>
    ::GetPixelTypeNameString()
  {
    return std::string("double");
  }

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxDisplacementFieldItkImageFilterSink.hxx"
#endif
#endif // #define selxDisplacementFieldItkImageFilterSink_h