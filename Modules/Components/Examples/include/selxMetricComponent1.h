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

#ifndef itkMetricComponent1_h
#define itkMetricComponent1_h

#include "selxSuperElastixComponent.h"
#include "selxExamplesInterfaces.h"

namespace selx
{
class MetricComponent1 :
  public SuperElastixComponent<
  Accepting< TransformedImageInterface >,
  Providing< MetricValueInterface >
  >
{
public:

  /** Standard class typedefs. */
  typedef MetricComponent1              Self;
  typedef ComponentBase                 Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  typedef Superclass::CriteriaType  CriteriaType;
  typedef Superclass::CriterionType CriterionType;

  MetricComponent1( const std::string & name, LoggerImpl & logger ) : SuperElastixComponent( name, logger ) {}
  virtual ~MetricComponent1(){}
  virtual int Accept( TransformedImageInterface::Pointer ) override;

  virtual int GetValue() override { return 0; }

  //std::string GetComponentTypeAsString() const;
  //static const char * GetName(){ return "MetricComponent1"; };
  static const char * GetDescription(){ return "Example Metric Component 1"; }

private:

  MetricComponent1( const Self & ); // purposely not implemented
  void operator=( const Self & );   // purposely not implemented

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  //virtual bool MeetsCriteria(const CriteriaType&);
};
} // end namespace selx

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkMetricComponent1.hxx"
//#endif

#endif
