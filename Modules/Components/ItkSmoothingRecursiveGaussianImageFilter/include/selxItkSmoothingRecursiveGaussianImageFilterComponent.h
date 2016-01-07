#ifndef selxItkSmoothingRecursiveGaussianImageFilterComponent_h
#define selxItkSmoothingRecursiveGaussianImageFilterComponent_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  class ItkSmoothingRecursiveGaussianImageFilterComponent : 
    public Implements<
    Accepting< itkProcessObjectInterface >,
    Providing< itkProcessObjectInterface >
    >
  {
  public:
    elxNewMacro(ItkSmoothingRecursiveGaussianImageFilterComponent, ComponentBase);

    ItkSmoothingRecursiveGaussianImageFilterComponent();
    virtual ~ItkSmoothingRecursiveGaussianImageFilterComponent();

    typedef itk::SmoothingRecursiveGaussianImageFilter<itk::Image<double, 3>, itk::Image<double, 3>> TheItkFilterType;
    
    int Set(itkProcessObjectInterface*);
    itk::ProcessObject::Pointer GetItkProcessObject();
    //int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion);    
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "ItkSmoothingRecursiveGaussianImageFilter Component"; };
  private:
    TheItkFilterType::Pointer m_theItkFilter;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h