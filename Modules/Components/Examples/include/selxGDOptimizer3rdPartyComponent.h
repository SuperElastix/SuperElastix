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

#ifndef GDOptimizer3rdPartyComponent_h
#define GDOptimizer3rdPartyComponent_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "selxExample3rdPartyCode.h"
#include "selxMetric3rdPartyWrapper.h"
#include <string.h>

namespace selx
{

  class GDOptimizer3rdPartyComponent : 
    public SuperElastixComponent<
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
    virtual int Set(MetricValueInterface*) override;
    virtual int Set(MetricDerivativeInterface*) override;
    virtual int Update() override;
    //virtual bool MeetsCriteria(const CriteriaType &criteria);
    virtual bool MeetsCriterion(const CriterionType &criterion) override;
    
    //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
    static const char * GetDescription() { return "GD Optimizer 3rd Party Component"; };
  };
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h