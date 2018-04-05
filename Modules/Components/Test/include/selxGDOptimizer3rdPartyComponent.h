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

#include "selxSuperElastixComponent.h"
#include "selxExamplesInterfaces.h"
#include "selxExample3rdPartyCode.h"
#include "selxMetric3rdPartyWrapper.h"
#include <string.h>

namespace selx
{
class GDOptimizer3rdPartyComponent :
  public SuperElastixComponent<
  Accepting< MetricValueInterface, MetricDerivativeInterface >,
  Providing< OptimizerUpdateInterface >
  >
{
public:

  /** Standard class typedefs. */
  typedef GDOptimizer3rdPartyComponent Self;

  typedef SuperElastixComponent< Accepting< MetricValueInterface, MetricDerivativeInterface >, Providing< OptimizerUpdateInterface   >> Superclass;
  typedef std::shared_ptr< Self >                                                                                                       Pointer;
  typedef std::shared_ptr< const Self >                                                                                                 ConstPointer;

  GDOptimizer3rdPartyComponent( const std::string & name, LoggerImpl & logger );
  virtual ~GDOptimizer3rdPartyComponent();
  Example3rdParty::GDOptimizer3rdParty * theImplementation;
  Metric3rdPartyWrapper *                MetricObject;

  virtual int Accept( MetricValueInterface::Pointer ) override;

  virtual int Accept( MetricDerivativeInterface::Pointer ) override;

  virtual int Update() override;

  //virtual bool MeetsCriteria(const CriteriaType &criteria);
  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  //static const char * GetName() { return "GDOptimizer3rdPartyComponent"; } ;
  static const char * GetDescription() { return "GD Optimizer 3rd Party Component"; }
};
} //end namespace selx
#endif // #define GDOptimizer3rdPartyComponent_h
