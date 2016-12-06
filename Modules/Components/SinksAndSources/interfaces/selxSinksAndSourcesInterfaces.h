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

#ifndef selxSinksAndSourcesInterfaces_h
#define selxSinksAndSourcesInterfaces_h

#include "selxInterfaces.h"
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

  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) = 0;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) = 0;
};

class SinkInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // By this interface only Sink Components can to talk to the Overlord
  // How specific Sink Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  typedef itk::DataObject::Pointer DataObjectPointer;
  virtual void SetMiniPipelineOutput( DataObjectPointer ) = 0;
  virtual DataObjectPointer GetMiniPipelineOutput( void ) = 0;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) = 0;

  virtual DataObjectPointer GetInitializedOutput( void ) = 0;
};



template< >
struct Properties< SourceInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "SourceInterface" } };
  }
};

template< >
struct Properties< SinkInterface >
{
  static const std::map< std::string, std::string > Get()
  {
    return { { keys::NameOfInterface, "SinkInterface" } };
  }
};

} // end namespace selx

#endif // #define selxSinksAndSourcesInterfaces_h
