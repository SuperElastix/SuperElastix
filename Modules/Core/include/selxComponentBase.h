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

#include "selxLoggerImpl.h"

namespace selx
{
class ComponentBase
{
public:

  //ComponentBase() = delete;
  ComponentBase();
  ComponentBase( const std::string & name, LoggerImpl & logger );
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

  // Will be implemented in SuperElastixComponent so that any interface can add a GetComponentName method without the need to implement that in each component individually.
  virtual std::string GetComponentName() const = 0;

  virtual int AcceptConnectionFrom( Pointer, const InterfaceCriteriaType ) = 0;

  virtual int AcceptConnectionFrom( Pointer ) = 0;

  /**
  Check if current components satisfies the properties listed in the Blueprint.

  During network construction phase, each property key-value pair of the 
  blueprint serves as an exclusion criterion whether a certain component 
  of the component database is eligible for the specific node in the graph.
  One-by-one and in arbitrary order, MeetsCriterion handles each property 
  key-value pair, which may be regular properties such as 
  "NumberOfIterations" : ["100"] or template properties such 
  as "Dimensionality": "3".

  Typically, MeetsCriterion is implemented by:
    - checking if the component recognizes the key;
	  - if key is a template property it must match the value of that of the component, otherwise false is returned
	  - if the key is a regular property, the value is saved (or assigned to underlying class variables). True is returned
	- if the key is not recognized, false is returned.
  */
  virtual bool MeetsCriterion( const CriterionType & criterion ) = 0;

  virtual InterfaceStatus CanAcceptConnectionFrom( ConstPointer, const InterfaceCriteriaType ) = 0;

  virtual unsigned int CountAcceptingInterfaces( const InterfaceCriteriaType ) = 0;

  virtual unsigned int CountProvidingInterfaces( const InterfaceCriteriaType ) = 0;

  //virtual const std::map< std::string, std::string >  TemplateProperties(); //TODO should be overridden

  // Each component is checked if its required connections are made after all handshakes.
  // SuperElastixComponent provides a default implementation which may be overridden by the component developer
  virtual bool ConnectionsSatisfied() = 0;

  void Cite()
  {
    if(!this->m_HowToCite.empty()) {
      this->m_Logger.Log(LogLevel::INF, "How to cite {0}: {1}", this->m_Name, this->m_HowToCite);
    }
  }

  template <typename ... Args> void Trace(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::TRC, fmt, args ... );
  }

  template <typename ... Args> void Error(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::ERR, fmt, args ... );
  }

  template <typename ... Args> void Debug(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::DBG, fmt, args ... );
  }

  template <typename ... Args> void Info(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::INF, fmt, args ... );
  }

  template <typename ... Args> void Warning(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::WRN, fmt, args ... );
  }

  template <typename ... Args> void Critical(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::CRT, fmt, args ... );
  }

  template <typename ... Args> void OFF(const std::string& fmt, const Args& ... args)
  {
    this->m_Logger.Log( LogLevel::OFF, fmt, args ... );
  }

  const std::string m_Name;
  std::string m_HowToCite;
  LoggerImpl & m_Logger;

};
} // end namespace selx

#endif // ComponentBase_h
