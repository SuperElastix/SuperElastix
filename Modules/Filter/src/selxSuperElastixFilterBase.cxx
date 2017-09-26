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

#include "selxSuperElastixFilterBase.h"
#include "selxNetworkBuilder.h"
#include "selxNetworkBuilderFactory.h"

namespace selx
{
/**
 * ********************* Constructor *********************
 */

SuperElastixFilterBase
::SuperElastixFilterBase() :
  m_IsConnected( false ),
  m_AllUniqueComponents( false )
{
  this->m_Blueprint = nullptr;
  this->m_Logger = Logger::New();
} // end Constructor

bool
SuperElastixFilterBase
::ParseBlueprint()
{
  if( ( this->m_Blueprint->GetMTime() > this->GetMTime() || !this->m_NetworkBuilder ) )
  {
    m_NetworkBuilder = m_NetworkBuilderFactory->New( this->m_Logger->GetLogger(), this->m_Blueprint->GetBlueprint() );
    this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
  }
  return this->m_AllUniqueComponents;
}


/**
* ********************* GenerateOutputInformation *********************
*/

void
SuperElastixFilterBase
::GenerateOutputInformation()
{
  /*
  * Override the ProcessObject default implementation, since outputs may be different types and
  * therefore the output information must come from the sink components.
  */

  // TODO this method should be revised together with NetworkBuilder->Configure(). A functionality
  // is foreseen in which the user may provide a minimalistic configuration (blueprint) and in
  // which other criteria to select a component are derived from neighboring components.
  // E.g. we could implement source nodes that provides ImageReaders of multiple dimensions.
  // At connecting these reader, the dimensionality is known from the data and the source
  // component can pass the dimensionality as an additional criterion to the component selectors
  // downstream. By (re-)configuring these, unique components may be selected and their requirements
  // are passed further down stream.
  // Eventually configuration boils down to a while loop that repeatedly tries to narrow down
  // the component selectors until no more unique components can be found.
  if( !this->m_Blueprint )
  {
    //TODO: remove this check here by making m_Blueprint primary input
    itkExceptionMacro( << "Setting a BlueprintImpl is required first." )
  }

  this->ParseBlueprint();

  // Handle inputs:
  auto                                       inputNames = this->GetInputNames();
  NetworkBuilderBase::SourceInterfaceMapType sources = this->m_NetworkBuilder->GetSourceInterfaces();
  for( const auto & nameAndInterface : sources )
  {
    auto inputName = std::find( inputNames.begin(), inputNames.end(), nameAndInterface.first );

    if( inputName == inputNames.end() )
    {
      // or should we catch and rethrow nameAndInterface.second->SetMiniPipelineInput(this->GetInput(nameAndInterface.first)); ?
      itkExceptionMacro( << "SuperElastixFilter requires the input " "" << nameAndInterface.first << "" " for the Source Component with that name" )
    }

    nameAndInterface.second->SetMiniPipelineInput( this->GetInput( nameAndInterface.first ) );
    inputNames.erase( inputName );
  }
  if( inputNames.size() > 0 )
  {
    std::stringstream msg;
    msg << "These inputs were given, but not used by any Source Component: " << std::endl;
    for( auto & unusedInputName : inputNames )
    {
      msg << unusedInputName << std::endl;
    }

    itkExceptionMacro( << msg.str() )
    //throw std::runtime_error(msg.str());
  }

  // Handle outputs:
  auto                                     usedOutputs = this->GetOutputNames();
  NetworkBuilderBase::SinkInterfaceMapType sinks       = this->m_NetworkBuilder->GetSinkInterfaces();
  for( const auto & nameAndInterface : sinks )
  {
    auto foundIndex = std::find( usedOutputs.begin(), usedOutputs.end(), nameAndInterface.first );

    if( foundIndex == usedOutputs.end() )
    {
      // or should we catch and rethrow nameAndInterface.second->SetMiniPipelineOutput(this->GetOutput(nameAndInterface.first)); ?
      itkExceptionMacro( << "SuperElastixFilter requires the output " "" << nameAndInterface.first << "" " for the Sink Component with that name" )
    }
    // This (empty) Output DataObject is known to the outside of the SuperElastixFilter and might be connected to an itk pipeline.
    // To keep the pipeline intact we need to propagate the DataObject upstream. Additional information such as requested region is preserved as well.
    // nameAndInterface.second->SetMiniPipelineOutput( this->GetOutput( nameAndInterface.first ) );
    usedOutputs.erase( foundIndex );
  }
  if( usedOutputs.size() > 0 )
  {
    std::stringstream msg;
    msg << "These outputs are connected, but not used by any Sink Component: " << std::endl;
    for( auto & unusedOutput : usedOutputs )
    {
      msg << unusedOutput << std::endl;
    }
    itkExceptionMacro( << msg.str() )
  }

  this->m_IsConnected =  this->m_NetworkBuilder->ConnectComponents();

  std::cout << "Connecting Components: " << ( this->m_IsConnected ? "succeeded" : "failed" ) << std::endl;

  bool connectionSatisfied = this->m_NetworkBuilder->CheckConnectionsSatisfied();
  if( connectionSatisfied == false )
  {
    itkExceptionMacro( << "One or more components has unsatisfied connections" )
  }

  std::cout << "All Components are satisfied with their connections" << std::endl << std::endl;

  for( const auto & nameAndInterface : sinks )
  {
    // Update information: ask the mini pipeline what the size of the data will be
    nameAndInterface.second->GetMiniPipelineOutput()->UpdateOutputInformation();
    // Put the information into the Filter's output Objects by grafting
    this->GetOutput( nameAndInterface.first )->Graft( nameAndInterface.second->GetMiniPipelineOutput() );
  }
}


/**
 * ********************* GenerateData *********************
 */

void
SuperElastixFilterBase
::GenerateData( void )
{
  std::cout << "Executing Network:" << std::endl;

  auto fullyConfiguredNetwork = this->m_NetworkBuilder->GetRealizedNetwork();
  // delete the networkbuilder
  // this->m_NetworkBuilder = nullptr;

  // This calls controller components that take over the control flow if the itk pipeline is broken.
  fullyConfiguredNetwork.Execute();

  // Connect the itk pipeline.
  auto outputObjectsMap = fullyConfiguredNetwork.GetOutputObjectsMap();
  for( const auto & nameAndObject : outputObjectsMap )
  {
    // Here we force all output to be updated.
    // TODO: it might be desirable to leave parts of the mini pipeline 'outdated' for memory/speed reasons and only update if requested downsteam.
    nameAndObject.second->Update();
    this->GetOutput( nameAndObject.first )->Graft( nameAndObject.second );
  }
}


SuperElastixFilterBase::AnyFileReaderType::Pointer
SuperElastixFilterBase
::GetInputFileReader( const DataObjectIdentifierType & inputName )
{
  //TODO: Before we can get the reader the BlueprintImpl needs to set and applied in the NetworkBuilder.
  // This is not like the itk pipeline philosophy
  if( !this->ParseBlueprint() )
  {
    itkExceptionMacro( << "BlueprintImpl was not sufficiently specified to build a network." )
  }
  return this->m_NetworkBuilder->GetInputFileReader( inputName );
}


SuperElastixFilterBase::AnyFileWriterType::Pointer
SuperElastixFilterBase
::GetOutputFileWriter( const DataObjectIdentifierType & outputName )
{
  //TODO: Before we can get the reader the BlueprintImpl needs to set and applied in the NetworkBuilder.
  // This is not like the itk pipeline philosophy
  if( !this->ParseBlueprint() )
  {
    itkExceptionMacro( << "BlueprintImpl was not sufficiently specified to build a network." )
  }

  return this->m_NetworkBuilder->GetOutputFileWriter( outputName );
}


void
SuperElastixFilterBase
::SetInput( const DataObjectIdentifierType & inputName, itk::DataObject * input )
{
  Superclass::SetInput( inputName, input );
  this->Modified();
}


SuperElastixFilterBase::OutputDataType
*
SuperElastixFilterBase
::GetOutput( const DataObjectIdentifierType & outputName )
{
  OutputDataType * output = Superclass::GetOutput( outputName );
  if( output != nullptr ) // if an output already exists, return it
  {
    return output;
  }
  else  // otherwise ask the sink component to initialize an output of the right type (the sink knows what type that is).
  {
    if( !this->ParseBlueprint() )
    {
      itkExceptionMacro( << "BlueprintImpl was not sufficiently specified to build a network." )
    }
    OutputDataType::Pointer newOutput = this->m_NetworkBuilder->GetInitializedOutput( outputName );
    this->Modified();

    Superclass::SetOutput( outputName, newOutput );

    return newOutput;
  }
}


void
SuperElastixFilterBase
::Update( void )
{
  //this->SetPrimaryOutput()
  this->GenerateOutputInformation();
  this->GenerateData();
}

} // namespace elx
