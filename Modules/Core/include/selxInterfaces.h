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

#ifndef selxInterfaces_h
#define selxInterfaces_h

#include "itkDataObject.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

namespace selx
{
// Define the providing interfaces abstractly

class SourceInterface
{
  // A special interface: the NetworkBuilder checks components for this type of interface.
  // By this interface only Source Components can to talk to the NetworkBuilder.
  // How specific Source Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  using Pointer = std::shared_ptr< SourceInterface >;
  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) = 0;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

class SinkInterface
{
  // A special interface: the NetworkBuilder checks components for this type of interface.
  // By this interface only Sink Components can to talk to the NetworkBuilder
  // How specific Sink Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  using Pointer = std::shared_ptr< SinkInterface >;
  typedef itk::DataObject::Pointer DataObjectPointer;
  virtual void SetMiniPipelineOutput( DataObjectPointer ) = 0;
  virtual DataObjectPointer GetMiniPipelineOutput( void ) = 0;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) = 0;

  virtual DataObjectPointer GetInitializedOutput( void ) = 0;

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

class UpdateInterface
{

public:

  using Pointer = std::shared_ptr< UpdateInterface >;

  // This interface is used to execute of the network. The components
  // are executed in topologically sorted order from input to output.
  virtual void Update() {};

  // This interface is run on all components prior to updating the network.
  // Components can use this interface do setup internal state after all
  // connections in the network has been set.
  virtual void BeforeUpdate() {};

  // GetComponentName is implemented in the SuperElastixComponent class and does not need to be implemented by each component individually.
  virtual std::string GetComponentName() const = 0;
};

} // end namespace selx

#endif // #define selxInterfaces_h
