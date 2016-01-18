#ifndef SSDMetric3rdPartyComponent_h
#define SSDMetric3rdPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example3rdPartyCode.h"

namespace selx
{
  // SSDMetric3rdPartyComponent provides a value and a derivative
  class SSDMetric3rdPartyComponent : 
    public Implements<
    Accepting<>,
    Providing< MetricDerivativeInterface, MetricValueInterface>
    >
  {
  public:
    /** Standard class typedefs. */
    typedef SSDMetric3rdPartyComponent                        Self;
    typedef ComponentBase Superclass;
    typedef itk::SmartPointer< Self >             Pointer;
    typedef itk::SmartPointer< const Self >       ConstPointer;

    /** New macro for creation of through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(SSDMetric3rdPartyComponent, Superclass);

    SSDMetric3rdPartyComponent();
    virtual ~SSDMetric3rdPartyComponent();
    Example3rdParty::SSDMetric3rdParty* theImplementation;
    virtual int GetValue() override;
    virtual int GetDerivative() override;
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    static const char * GetName(){ return "SSDMetric3rdPartyComponent"; }
    static const char * GetDescription(){ return "SSD Metric 3rd Party Component"; };
  };
} //end namespace selx
#endif // #define SSDMetric3rdPartyComponent_h