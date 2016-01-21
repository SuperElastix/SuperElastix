#ifndef selxItkImageSink_h
#define selxItkImageSink_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "elxMacro.h"
#include "itkImageFileWriter.h"

namespace selx
{
  class ItkImageSinkComponent :
    public Implements <
    Accepting< itkImageSourceInterface<3,double> >,
    Providing < SinkInterface >
    >
  {
  public:
    elxNewMacro(ItkImageSinkComponent, ComponentBase);

    ItkImageSinkComponent();
    virtual ~ItkImageSinkComponent();

    typedef itk::ImageSource<itk::Image<double, 3>> ItkImageSourceType;

    virtual int Set(itkImageSourceInterface<3, double>*) override;
    virtual bool ConnectToOverlordSink(itk::Object::Pointer) override;

    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    static const char * GetDescription() { return "ItkImageSink Component"; };
  private:
    itk::ProcessObject::Pointer m_Sink;
    itk::ImageFileWriter<itk::Image<double, 3>>::Pointer m_SinkWriter;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h