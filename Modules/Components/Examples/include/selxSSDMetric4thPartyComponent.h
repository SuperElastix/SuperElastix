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

#ifndef SSDMetric4thPartyComponent_h
#define SSDMetric4thPartyComponent_h

#include "selxSuperElastixComponent.h"
#include "selxExamplesInterfaces.h"
#include "selxExample4thPartyCode.h"

namespace selx
{
// SSDMetric4thPartyComponent provides only a value and not a derivative
class SSDMetric4thPartyComponent :
  public SuperElastixComponent<
  Accepting< >,
  Providing< MetricValueInterface >
  >
{
public:

  /** Standard class typedefs. */
  typedef SSDMetric4thPartyComponent Self;
  typedef SuperElastixComponent<
    Accepting< >,
    Providing< MetricValueInterface >
    >                 Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  SSDMetric4thPartyComponent( const std::string & name, LoggerImpl & logger );
  virtual ~SSDMetric4thPartyComponent();
  Example4thParty::SSDMetric4thParty * theImplementation;
  virtual int GetValue() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  //virtual bool MeetsCriteria(const CriteriaType &criteria);
  //static const char * GetName(){ return "SSDMetric4thPartyComponent"; };
  static const char * GetDescription(){ return "SSD Metric 4th Party Component"; }
};
} //end namespace selx
#endif // #define SSDMetric4thPartyComponent_h
