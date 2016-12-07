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

#include "itkLightProcessObject.h"
#include "itkObjectFactory.h"
#include "itkVectorContainer.h"

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
#include "selxComponentFactory.h"
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

namespace selx
{
class NetworkBuilder
{
  // TODO: consider renaming NetworkBuilder to NetworkBuilder (?). Its output should be a (light weight) ComponentContainer with 1 Execute button. All other data such as graphs and selectors can be deleted.
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
  ~NetworkBuilder() {}

  /** Read configuration at the blueprints nodes and edges and return true if all components could be uniquely selected*/
  bool Configure();

  /** if all components are uniquely selected, they can be connected */
  bool ConnectComponents();

  /** Run the (registration) algorithm */
  void Execute();

  SourceInterfaceMapType GetSourceInterfaces();

  SinkInterfaceMapType GetSinkInterfaces();

  AnyFileReader::Pointer GetInputFileReader( const ComponentNameType & );

  AnyFileWriter::Pointer GetOutputFileWriter( const ComponentNameType & );

  SinkInterface::DataObjectPointer GetInitializedOutput( const NetworkBuilder::ComponentNameType & );

protected:

private:

  typedef ComponentBase::CriteriaType       CriteriaType;
  typedef ComponentBase::CriterionType      CriterionType;
  typedef ComponentBase::ParameterValueType ParameterValueType;

  typedef ComponentSelector::Pointer ComponentSelectorPointer;

  typedef Blueprint::ComponentNamesType                           ComponentNamesType;
  typedef std::map< ComponentNameType, ComponentSelectorPointer > ComponentSelectorContainerType;
  typedef ComponentSelectorContainerType::iterator                ComponentSelectorIteratorType;

  /** Read configuration at the blueprints nodes and try to find instantiated components */
  void ApplyComponentConfiguration();

  /** Read configuration at the blueprints edges and try to find instantiated components */
  void ApplyConnectionConfiguration();

  /** For all uniquely selected components test handshake to non-uniquely selected components */
  void PropagateConnectionsWithUniqueComponents();

  /** See which components need more configuration criteria */
  ComponentNamesType GetNonUniqueComponentNames();

  //TODO make const correct
  //NetworkBuilder should be constructed with a blueprint.
  //Blueprint::ConstPointer m_Blueprint;
  //Blueprint const * m_Blueprint;
  std::shared_ptr< const Blueprint > m_Blueprint;

  // A selector for each node, that each can hold multiple instantiated components. Ultimately is should be 1 component each.
  ComponentSelectorContainerType m_ComponentSelectorContainer;
  bool                           m_isConfigured;
};
} // end namespace selx

#endif // NetworkBuilder_h
