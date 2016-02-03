#ifndef selxItkImageSourceFixed_h
#define selxItkImageSourceFixed_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  class ItkImageSourceFixedComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface, itkImageSourceFixedInterface<3,double > >
    >
  {
  public:
    elxNewMacro(ItkImageSourceFixedComponent, ComponentBase);

    ItkImageSourceFixedComponent();
    virtual ~ItkImageSourceFixedComponent();

    typedef itk::ImageSource<itk::Image<double, 3>> ItkImageSourceFixedType;
    

    virtual ItkImageSourceFixedType::Pointer GetItkImageSourceFixed() override;
    virtual bool ConnectToOverlordSource(itk::Object::Pointer) override;

    //int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "ItkImageSourceFixed Component"; };
  private:
    ItkImageSourceFixedType::Pointer m_Source;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h