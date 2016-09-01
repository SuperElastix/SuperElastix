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

#ifndef selxItkGradientDescentOptimizerv4Component_h
#define selxItkGradientDescentOptimizerv4Component_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "itkGradientDescentOptimizerv4.h"
#include <string.h>
#include "selxMacro.h"


namespace selx
{
template< class InternalComputationValueType >
class ItkGradientDescentOptimizerv4Component :
  public SuperElastixComponent<
  Accepting< >,
  Providing< itkOptimizerv4Interface< InternalComputationValueType >>
  >
{
public:

  selxNewMacro( ItkGradientDescentOptimizerv4Component, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkGradientDescentOptimizerv4Component();
  virtual ~ItkGradientDescentOptimizerv4Component();

  /**  Type of the optimizer. */
  typedef typename itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > OptimizerType;
  typedef typename OptimizerType::Pointer                                                   Optimizerv4Pointer;

  typedef typename itk::GradientDescentOptimizerv4Template< InternalComputationValueType > GradientDescentOptimizerv4Type;

  virtual Optimizerv4Pointer GetItkOptimizerv4() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkGradientDescentOptimizerv4"; } ;
  static const char * GetDescription() { return "ItkGradientDescentOptimizerv4 Component"; }

private:

  typename GradientDescentOptimizerv4Type::Pointer m_Optimizer;

protected:

  /* The following struct returns the string name of computation type */
  /* default implementation */
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkGradientDescentOptimizerv4.hxx"
#endif
#endif // #define selxItkGradientDescentOptimizerv4Component_h
