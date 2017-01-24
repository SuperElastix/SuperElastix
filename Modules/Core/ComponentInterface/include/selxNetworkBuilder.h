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

#ifndef NetworkBuilder_h
#define NetworkBuilder_h

#include "selxNetworkBuilderBase.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>

#include "selxMacro.h"
#include "selxBlueprint.h"
#include "selxNetworkContainer.h"
#include "selxComponentFactory.h"
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"

namespace selx
{
  template<class ComponentList>
  class NetworkBuilder : public NetworkBuilderBase
{
  // The NetworkBuilder takes care of the at run time realization of the algorithm network that is described by the Blueprint.
  // The output, GetRealizedNetwork(), is a (light weight) ComponentContainer with 1 Execute button that is self-contained to run the registration algorithm. 
  // After obtaining the RealizedNetwork(), the NetworkBuilder object can be deleted in order to free memory, releasing all internal/intermediate data of the configuration process.
public:



  typedef Blueprint::ComponentNameType ComponentNameType;
  typedef std::map<
    std::string, SourceInterface * > SourceInterfaceMapType;
  typedef std::map<
    std::string, SinkInterface * > SinkInterfaceMapType;

  typedef std::map<
    std::string, RegistrationControllerStartInterface * > RegistrationControllerStartInterfaceMapType;

  NetworkBuilder( std::shared_ptr< Blueprint > blueprint );
  NetworkBuilder( Blueprint * blueprint );
  NetworkBuilder();
  virtual ~NetworkBuilder() {};

  virtual bool AddBlueprint(std::shared_ptr< Blueprint > blueprint);

  /** Read configuration at the blueprints nodes and edges and return true if all components could be uniquely selected*/
  virtual bool Configure();

  /** if all components are uniquely selected, they can be connected */
  virtual bool ConnectComponents();

  virtual NetworkContainer GetRealizedNetwork();

  virtual SourceInterfaceMapType GetSourceInterfaces();

  virtual SinkInterfaceMapType GetSinkInterfaces();

  virtual AnyFileReader::Pointer GetInputFileReader(const ComponentNameType &);

  virtual AnyFileWriter::Pointer GetOutputFileWriter(const ComponentNameType &);

  virtual SinkInterface::DataObjectPointer GetInitializedOutput(const NetworkBuilderBase::ComponentNameType &);

protected:

  typedef ComponentBase::CriteriaType       CriteriaType;
  typedef ComponentBase::CriterionType      CriterionType;
  typedef ComponentBase::ParameterValueType ParameterValueType;

  typedef typename ComponentSelector<ComponentList> ComponentSelectorType;
  typedef typename ComponentSelectorType::Pointer ComponentSelectorPointer;

  typedef std::map< ComponentNameType, ComponentSelectorPointer > ComponentSelectorContainerType;
  typedef typename ComponentSelectorContainerType::iterator                ComponentSelectorIteratorType;

  /** Read configuration at the blueprints nodes and try to find instantiated components */
  virtual void ApplyComponentConfiguration();

  /** Read configuration at the blueprints edges and try to find instantiated components */
  virtual void ApplyConnectionConfiguration();

  /** For all uniquely selected components test handshake to non-uniquely selected components */
  virtual void PropagateConnectionsWithUniqueComponents();

  /** See which components need more configuration criteria */
  virtual ComponentNamesType GetNonUniqueComponentNames();

  //TODO make const correct
  //NetworkBuilder should be constructed with a blueprint.
  //Blueprint::ConstPointer m_Blueprint;
  //Blueprint const * m_Blueprint;
  std::shared_ptr< Blueprint > m_Blueprint;

  // A selector for each node, that each can hold multiple instantiated components. Ultimately is should be 1 component each.
  ComponentSelectorContainerType m_ComponentSelectorContainer;
  bool                           m_isConfigured;

private:

};
} // end namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNetworkBuilder.hxx"
#endif

#endif // NetworkBuilder_h
