#ifndef selxItkSmoothingRecursiveGaussianImageFilterComponent_h
#define selxItkSmoothingRecursiveGaussianImageFilterComponent_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkImageSource.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  class ItkSmoothingRecursiveGaussianImageFilterComponent : 
    public Implements<
    Accepting< itkImageSourceInterface >,
    Providing< itkImageSourceInterface >
    >
    // TODO: see if itkImageSourceInterface is the right way to connect itk filters..
    //Accepting< itkProcessObjectInterface, itkImageToImageFilterInterface >,
    //Providing< itkProcessObjectInterface, itkImageToImageFilterInterface >
    //>
  {
  public:
    elxNewMacro(ItkSmoothingRecursiveGaussianImageFilterComponent, ComponentBase);

    ItkSmoothingRecursiveGaussianImageFilterComponent();
    virtual ~ItkSmoothingRecursiveGaussianImageFilterComponent();

    typedef itk::SmoothingRecursiveGaussianImageFilter<itk::Image<double, 3>, itk::Image<double, 3>> TheItkFilterType;
    typedef itk::ImageSource<itk::Image<double, 3>> ItkImageSourceType;

    // TODO: see if itkImageSourceInterface is the right way to connect itk filters..
    /*
    virtual int Set(itkProcessObjectInterface*) override;
    virtual itk::ProcessObject::Pointer GetItkProcessObject() override;

    virtual int Set(itkImageToImageFilterInterface*) override;
    virtual itk::ImageToImageFilter<itk::Image<double, 3>, itk::Image<double, 3>>::Pointer GetItkImageToImageFilter() override;
    */
    virtual int Set(itkImageSourceInterface*) override;
    virtual ItkImageSourceType::Pointer GetItkImageSource() override;

    //int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;    
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "ItkSmoothingRecursiveGaussianImageFilter Component"; };
  private:
    TheItkFilterType::Pointer m_theItkFilter;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h