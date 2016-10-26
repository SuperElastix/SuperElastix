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


#ifndef selxLogComponent_h
#define selxLogComponent_h

#include "selxInterfaces.h"
#include "selxSuperElastixComponent.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

namespace selx 
{
  
class LogComponent : 
  public SuperElastixComponent<
    Accepting< >,
    Providing
    <
      LogInterface
    >
  >
{
public:

  //@kasper: somehow the selxNewMacro does not work. The itkNewMacro, however works. Something to figure out why...
  //selxNewMacro( LogComponent, ComponentBase );

  /** Standard class typedefs. */
  typedef LogComponent                Self;
  typedef ComponentBase                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** New macro for creation of through the object factory. */
  //selxNewMacro(Self, ComponentBase);
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LogComponent, ComponentBase);

  LogComponent();
  virtual ~LogComponent(){};

  virtual std::stringstream Log( boost::log::trivial::severity_level severity_level ) override;

  virtual bool MeetsCriterion(const CriterionType & criterion) override;

  static const char * GetDescription() { return "Log Component"; }
private:
  
  boost::log::trivial::severity_level m_LogLevel;
};

}

#endif // selxLogComponent_h