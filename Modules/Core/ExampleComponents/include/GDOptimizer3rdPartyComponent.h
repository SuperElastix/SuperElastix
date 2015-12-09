#ifndef GDOptimizer3rdPartyComponent_h
#define GDOptimizer3rdPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.h"
#include "Example3rdPartyCode.h"
#include "Metric3rdPartyWrapper.h"
#include <string.h>

namespace selx
{

  class GDOptimizer3rdPartyComponent : 
    public Implements<
      Accepting< MetricValueInterface, MetricDerivativeInterface >,
      Providing< OptimizerUpdateInterface>
    >
  {
  public:
    /** Standard class typedefs. */
    typedef GDOptimizer3rdPartyComponent                        Self;
    typedef ComponentBase Superclass;
    typedef itk::SmartPointer< Self >             Pointer;
    typedef itk::SmartPointer< const Self >       ConstPointer;

    /** New macro for creation of through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(GDOptimizer3rdPartyComponent, Superclass);


    GDOptimizer3rdPartyComponent();
    virtual ~GDOptimizer3rdPartyComponent();
    Example3rdParty::GDOptimizer3rdParty* theImplementation;
    Metric3rdPartyWrapper* MetricObject;
    //virtual int ConnectFrom(const char *, ComponentBase*);
    int Set(MetricValueInterface*);
    int Set(MetricDerivativeInterface*);
    int Update();
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterium(const CriteriumType &criterium);
    
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "GD Optimizer 3rd Party Component"; };
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h