#ifndef selxItkImageSourceMoving_h
#define selxItkImageSourceMoving_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include <string.h>
#include "elxMacro.h"
namespace selx
{
  class ItkImageSourceMovingComponent : 
    public Implements<
    Accepting<>,
    Providing< SourceInterface, itkImageSourceMovingInterface<3,double > >
    >
  {
  public:
    elxNewMacro(ItkImageSourceMovingComponent, ComponentBase);

    ItkImageSourceMovingComponent();
    virtual ~ItkImageSourceMovingComponent();

    typedef itk::ImageSource<itk::Image<double, 3>> ItkImageSourceMovingType;
    

    virtual ItkImageSourceMovingType::Pointer GetItkImageSourceMoving() override;
    virtual bool ConnectToOverlordSource(itk::Object::Pointer) override;

    //int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "ItkImageSourceMoving Component"; };
  private:
    ItkImageSourceMovingType::Pointer m_Source;
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h