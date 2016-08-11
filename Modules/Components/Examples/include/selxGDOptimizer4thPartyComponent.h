/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical 
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef GDOptimizer4thPartyComponent_h
#define GDOptimizer4thPartyComponent_h

#include "selxComponentBase.h"
#include "selxInterfaces.hxx"
#include "selxExample4thPartyCode.h"
#include "selxMetric4thPartyWrapper.h"

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
    virtual int Set(MetricValueInterface*) override;
    virtual int Update() override;

    //template <class ConflictinUpdateInterface> virtual int Update() { return 5; };
    // "error" : member function templates cannot be virtual
    virtual int Update(ConflictinUpdateInterface*) override { return 5; };
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    //static const char * GetName(){ return "GDOptimizer4thPartyComponent"; };
    static const char * GetDescription(){ return "GD Optimizer 4th Party Component"; };
  };

} //end namespace selx
#endif // #define GDOptimizer4thPartyComponent_h