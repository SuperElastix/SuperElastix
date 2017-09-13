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
  // A special interface: the Overlord checks components for this type of interface.
  // By this interface only Source Components can to talk to the Overlord.
  // How specific Source Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  using Pointer = std::shared_ptr< SourceInterface >;
  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) = 0;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) = 0;
};

class SinkInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // By this interface only Sink Components can to talk to the Overlord
  // How specific Sink Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  using Pointer = std::shared_ptr< SinkInterface >;
  typedef itk::DataObject::Pointer DataObjectPointer;
  virtual void SetMiniPipelineOutput( DataObjectPointer ) = 0;
  virtual DataObjectPointer GetMiniPipelineOutput( void ) = 0;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) = 0;

  virtual DataObjectPointer GetInitializedOutput( void ) = 0;
};

class RegistrationControllerStartInterface
{
  // A special interface: the NetworkBuilder checks components for this type of interface.
  // This interface is to control the execution of the network

public:

  using Pointer                              = std::shared_ptr< RegistrationControllerStartInterface >;
  virtual void RegistrationControllerStart() = 0;
};

class RunRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  using Pointer                  = std::shared_ptr< RunRegistrationInterface >;
  virtual void RunRegistration() = 0;
};

class AfterRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  using Pointer                    = std::shared_ptr< AfterRegistrationInterface >;
  virtual void AfterRegistration() = 0;
};

class RunResolutionInterface
{
  // This interface is to control the execution of the network

public:

  using Pointer                = std::shared_ptr< RunResolutionInterface >;
  virtual bool RunResolution() = 0;
};

class ReconnectTransformInterface
{
  // This interface is to control the execution of the network

public:

  using Pointer                     = std::shared_ptr< ReconnectTransformInterface >;
  virtual void ReconnectTransform() = 0;
};
} // end namespace selx

#endif // #define selxInterfaces_h
