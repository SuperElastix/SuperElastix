#ifndef GDOptimizer4thPartyComponent_h
#define GDOptimizer4thPartyComponent_h

#include "ComponentBase.h"
#include "Interfaces.hxx"
#include "Example4thPartyCode.h"
#include "Metric4thPartyWrapper.h"

namespace selx
{
  // wrapping into components: 
  class GDOptimizer4thPartyComponent : 
    public Implements <
    Accepting< MetricValueInterface >,
    Providing < OptimizerUpdateInterface, ConflictinUpdateInterface >
    >

  {
  public:
    GDOptimizer4thPartyComponent();
    virtual ~GDOptimizer4thPartyComponent();
    Example4thParty::GDOptimizer4thParty* theImplementation;
    Metric4thPartyWrapper* MetricObject;
    //virtual int ConnectFrom(const char *, ComponentBase*);
    int Set(MetricValueInterface*);
    int Update();

    //template <class ConflictinUpdateInterface> virtual int Update() { return 5; };
    // "error" : member function templates cannot be virtual
    int Update(ConflictinUpdateInterface*) { return 5; };
    virtual bool MeetsCriteria(const CriteriaType &criteria) { return false; };
    static const char * GetName(){ return "GDOptimizer4thPartyComponent"; };
    static const char * GetDescription(){ return "GD Optimizer 4th Party Component"; };
  };

} //end namespace selx
#endif // #define GDOptimizer4thPartyComponent_h