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

#ifndef NetworkBuilderBase_h
#define NetworkBuilderBase_h

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>

#include "selxComponentSelector.h"
#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "selxInterfaceTraits.h"
#include "selxBlueprintImpl.h"
#include "selxNetworkContainer.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"
#include "selxLoggerImpl.h"

namespace selx
{
class NetworkBuilderBase
{
  // The NetworkBuilderBase takes care of the at run time realization of the algorithm network that is described by the BlueprintImpl.
  // The output, GetRealizedNetwork(), is a (light weight) ComponentContainer with 1 Execute button that is self-contained to run the registration algorithm.
  // After obtaining the RealizedNetwork(), the NetworkBuilderBase object can be deleted in order to free memory, releasing all internal/intermediate data of the configuration process.

public:

  typedef BlueprintImpl::ComponentNameType  ComponentNameType;
  typedef BlueprintImpl::ComponentNamesType ComponentNamesType;
  typedef std::map<
    std::string, SourceInterface::Pointer > SourceInterfaceMapType;
  typedef std::map<
    std::string, SinkInterface::Pointer > SinkInterfaceMapType;

  NetworkBuilderBase() {}

  virtual ~NetworkBuilderBase() {}

  virtual bool AddBlueprint( const BlueprintImpl & blueprint ) = 0;

  /** Read configuration at the blueprints nodes and edges and return true if all components could be uniquely selected*/
  virtual bool Configure() = 0;

  /** if all components are uniquely selected, they can be connected */
  virtual bool ConnectComponents() = 0;

  virtual bool CheckConnectionsSatisfied() = 0;

  virtual NetworkContainer GetRealizedNetwork() = 0;

  virtual SourceInterfaceMapType GetSourceInterfaces() = 0;

  virtual SinkInterfaceMapType GetSinkInterfaces() = 0;

  virtual AnyFileReader::Pointer GetInputFileReader( const ComponentNameType & ) = 0;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( const ComponentNameType & ) = 0;

  virtual SinkInterface::DataObjectPointer GetInitializedOutput( const NetworkBuilderBase::ComponentNameType & ) = 0;

  virtual void Cite() = 0;

private:
};
} // end namespace selx

#endif // NetworkBuilderBase_h
