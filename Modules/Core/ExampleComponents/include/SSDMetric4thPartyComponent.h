#ifndef SSDMetric4thPartyComponent_h
#define SSDMetric4thPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example4thPartyCode.h"

namespace selx
{
  // SSDMetric4thPartyComponent provides only a value and not a derivative
  class SSDMetric4thPartyComponent : 
    public Implements<
    Accepting<>,
    Providing< MetricValueInterface>
    >
  {
  public:
    /** Standard class typedefs. */
    typedef SSDMetric4thPartyComponent                        Self;
    typedef ComponentBase Superclass;
    typedef itk::SmartPointer< Self >             Pointer;
    typedef itk::SmartPointer< const Self >       ConstPointer;

    /** New macro for creation of through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(SSDMetric4thPartyComponent, Superclass);

    SSDMetric4thPartyComponent();
    virtual ~SSDMetric4thPartyComponent();
    Example4thParty::SSDMetric4thParty* theImplementation;
    int GetValue();
    virtual bool MeetsCriterium(const CriteriumType &criterium);
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    //static const char * GetName(){ return "SSDMetric4thPartyComponent"; };
    static const char * GetDescription(){ return "SSD Metric 4th Party Component"; };
    
  };
} //end namespace selx
#endif // #define SSDMetric4thPartyComponent_h