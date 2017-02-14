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

#ifndef ComponentBase_h
#define ComponentBase_h

#include "selxInterfaceStatus.h"
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <memory>

namespace selx
{
class ComponentBase
{
public:

  //ComponentBase() = delete;
  ComponentBase();
  ComponentBase( const std::string & name );
  virtual ~ComponentBase() {}

  typedef std::shared_ptr< ComponentBase >       Pointer;
  typedef std::shared_ptr< const ComponentBase > ConstPointer;
  typedef std::string                            ParameterKeyType;
  typedef std::vector< std::string >             ParameterValueType;
  //typedef std::map< ParameterKeyType, ParameterValueType >           ParameterMapType;
  //TODO choose uniform naming for Typedefs
  typedef std::map< ParameterKeyType, ParameterValueType >  CriteriaType;
  typedef std::pair< ParameterKeyType, ParameterValueType > CriterionType;

  typedef std::map< std::string, std::string > InterfaceCriteriaType;

  virtual int AcceptConnectionFrom( Pointer, const InterfaceCriteriaType ) = 0;

  virtual int AcceptConnectionFrom( Pointer ) = 0;

  virtual bool MeetsCriterion( const CriterionType & criterion ) = 0;

  virtual InterfaceStatus CanAcceptConnectionFrom( ConstPointer, const InterfaceCriteriaType ) = 0;

  virtual unsigned int CountAcceptingInterfaces( const InterfaceCriteriaType ) = 0;

  virtual unsigned int CountProvidingInterfaces( const InterfaceCriteriaType ) = 0;

  //virtual const std::map< std::string, std::string >  TemplateProperties(); //TODO should be overridden

  const std::string m_Name;
};
} // end namespace selx

#endif // ComponentBase_h
