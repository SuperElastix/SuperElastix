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
    /** Standard class typedefs. */
    typedef GDOptimizer4thPartyComponent                        Self;
    typedef ComponentBase Superclass;
    typedef itk::SmartPointer< Self >             Pointer;
    typedef itk::SmartPointer< const Self >       ConstPointer;

    /** New macro for creation of through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(GDOptimizer4thPartyComponent, Superclass);

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
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion);
    //static const char * GetName(){ return "GDOptimizer4thPartyComponent"; };
    static const char * GetDescription(){ return "GD Optimizer 4th Party Component"; };
  };

} //end namespace selx
#endif // #define GDOptimizer4thPartyComponent_h