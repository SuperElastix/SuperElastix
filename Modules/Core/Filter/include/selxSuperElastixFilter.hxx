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

#ifndef selxSuperElastixFilter_hxx
#define selxSuperElastixFilter_hxx

#include "selxSuperElastixFilter.h"
#include "selxRegisterComponentFactoriesByTypeList.h"

namespace selx
{
/**
 * ********************* Constructor *********************
 */

template< typename ComponentTypeList >
SuperElastixFilter< ComponentTypeList >
::SuperElastixFilter( void ) : m_InputConnectionModified( true ), m_OutputConnectionModified( true ), m_BlueprintConnectionModified( true ),
  m_IsConnected( false )
{
  this->m_Overlord = std::unique_ptr< Overlord >( new Overlord( this->m_Blueprint.Get() ) );

  RegisterFactoriesByTypeList< ComponentTypeList >::Register();

  //Disable "Primary" as required input
  this->SetRequiredInputNames( {} );
} // end Constructor


/**
* ********************* GenerateOutputInformation *********************
*/

template< typename ComponentTypeList >
void
SuperElastixFilter< ComponentTypeList >
::GenerateOutputInformation()
{
  /*
  * Override the ProcessObject default implementation, since outputs may be different types and
  * therefore the output information must come from the sink components.
  */

  // TODO this method should be revised together with overlord->Configure(). A functionality
  // is foreseen in which the user may provide a minimalistic configuration (blueprint) and in
  // which other criteria to select a component are derived from neighboring components.
  // E.g. we could implement source nodes that provides ImageReaders of multiple dimensions.
  // At connecting these reader, the dimensionality is known from the data and the source
  // component can pass the dimensionality as an additional criterion to the component selectors
  // downstream. By (re-)configuring these, unique components may be selected and their requirements
  // are passed further down stream.
  // Eventually configuration boils down to a while loop that repeatedly tries to narrow down
  // the component selectors until no more unique components can be found.

  bool allUniqueComponents = true;
  if( this->m_BlueprintConnectionModified == true )
  {
    this->m_Overlord->SetBlueprint( this->m_Blueprint );
    allUniqueComponents = this->m_Overlord->Configure();
  }

  if( ( m_InputConnectionModified == true ) || ( this->m_BlueprintConnectionModified == true ) )
  {
    auto                             usedInputs = this->GetInputNames();
    Overlord::SourceInterfaceMapType sources    = this->m_Overlord->GetSourceInterfaces();
    for( const auto & nameAndInterface : sources )
    {
      auto foundIndex = std::find( usedInputs.begin(), usedInputs.end(), nameAndInterface.first );

      if( foundIndex == usedInputs.end() )
      {
        // or should we catch and rethrow nameAndInterface.second->SetMiniPipelineInput(this->GetInput(nameAndInterface.first)); ?
        itkExceptionMacro( << "SuperElastixFilter requires the input " "" << nameAndInterface.first << "" " for the Source Component with that name" )
      }

      nameAndInterface.second->SetMiniPipelineInput( this->GetInput( nameAndInterface.first ) );
      usedInputs.erase( foundIndex );
    }
    if( usedInputs.size() > 0 )
    {
      std::stringstream msg;
      msg << "These inputs are connected, but not used by any Source Component: " << std::endl;
      for( auto & unusedInput : usedInputs )
      {
        msg << unusedInput << std::endl;
      }
      itkExceptionMacro( << msg.str() )
    }
  }

  if( ( m_OutputConnectionModified == true ) || ( this->m_BlueprintConnectionModified == true ) )
  {
    auto                           usedOutputs = this->GetOutputNames();
    Overlord::SinkInterfaceMapType sinks       = this->m_Overlord->GetSinkInterfaces();
    for( const auto & nameAndInterface : sinks )
    {
      auto foundIndex = std::find( usedOutputs.begin(), usedOutputs.end(), nameAndInterface.first );

      if( foundIndex == usedOutputs.end() )
      {
        // or should we catch and rethrow nameAndInterface.second->SetMiniPipelineOutput(this->GetOutput(nameAndInterface.first)); ?
        itkExceptionMacro( << "SuperElastixFilter requires the output " "" << nameAndInterface.first << "" " for the Sink Component with that name" )
      }
      nameAndInterface.second->SetMiniPipelineOutput( this->GetOutput( nameAndInterface.first ) );
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
  }

  if( allUniqueComponents == false ) // by setting inputs and outputs, settings could be derived to uniquely select the other components
  {
    allUniqueComponents = this->m_Overlord->Configure();
  }

  if( allUniqueComponents && !this->m_IsConnected )
  {
    this->m_IsConnected = this->m_Overlord->ConnectComponents();
    std::cout << "Connecting Components: " << (this->m_IsConnected ? "succeeded" : "failed") << std::endl << std::endl;
  }

  if( ( m_OutputConnectionModified == true ) || ( this->m_BlueprintConnectionModified == true ) )
  {
    Overlord::SinkInterfaceMapType sinks = this->m_Overlord->GetSinkInterfaces();
    for( const auto & nameAndInterface : sinks )
    {
      // Update information: ask the mini pipeline what the size of the data will be
      nameAndInterface.second->GetMiniPipelineOutput()->UpdateOutputInformation();
      // Put the information into the Filter's output Objects by grafting
      this->GetOutput( nameAndInterface.first )->Graft( nameAndInterface.second->GetMiniPipelineOutput() );
    }
  }

  this->m_BlueprintConnectionModified = false;
  this->m_InputConnectionModified     = false;
  this->m_OutputConnectionModified    = false;
}


/**
 * ********************* GenerateData *********************
 */

template< typename ComponentTypeList >
void
SuperElastixFilter< ComponentTypeList >
::GenerateData( void )
{
  std::cout << "Executing Network:" << std::endl;
  // This calls controller components that take over the control flow if the itk pipeline is broken.
  this->m_Overlord->Execute();

  Overlord::SinkInterfaceMapType sinks = this->m_Overlord->GetSinkInterfaces();
  for( const auto & nameAndInterface : sinks )
  {
    // Here we force all output to be updated.
    // TODO: it might be desirable to leave parts of the mini pipeline 'outdated' for memory/speed reasons and only update if requested downsteam.
    nameAndInterface.second->GetMiniPipelineOutput()->Update();
    this->GetOutput( nameAndInterface.first )->Graft( nameAndInterface.second->GetMiniPipelineOutput() );
  }
}


template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::AnyFileReaderType::Pointer
SuperElastixFilter< ComponentTypeList >
::GetInputFileReader( const DataObjectIdentifierType & inputName )
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the overlord.
  // This is not like the itk pipeline philosophy
  if( !this->m_Blueprint )
  {
    itkExceptionMacro( << "Setting a Blueprint is required first." )
  }
  this->m_Overlord->SetBlueprint( this->m_Blueprint );
  this->m_Overlord->Configure();

  return this->m_Overlord->GetInputFileReader( inputName );
}


template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::AnyFileWriterType::Pointer
SuperElastixFilter< ComponentTypeList >
::GetOutputFileWriter( const DataObjectIdentifierType & outputName )
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the overlord.
  // This is not like the itk pipeline philosophy
  if( !this->m_Blueprint )
  {
    itkExceptionMacro( << "Setting a Blueprint is required first." )
  }

  this->m_Overlord->SetBlueprint( this->m_Blueprint );
  this->m_Overlord->Configure();

  return this->m_Overlord->GetOutputFileWriter( outputName );
}


template< typename ComponentTypeList >
void
SuperElastixFilter< ComponentTypeList >
::SetInput( const DataObjectIdentifierType & inputName, itk::DataObject * input )
{
  Superclass::SetInput( inputName, input );
  this->m_InputConnectionModified = true;
}


template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::OutputDataType
* SuperElastixFilter< ComponentTypeList >
::GetOutput( const DataObjectIdentifierType &outputName )
{
  OutputDataType * output = Superclass::GetOutput( outputName );
  if( output != nullptr ) // if an output already exists, return it
  {
    return output;
  }
  else  // otherwise ask the sink component to initialize an output of the right type (the sink knows what type that is).
  {
    if( !this->m_Blueprint ) // to ask the sink it must be configured by a blueprint.
    {
      itkExceptionMacro( << "Setting a Blueprint is required first." )
    }

    this->m_Overlord->SetBlueprint( this->m_Blueprint );
    this->m_Overlord->Configure();
    this->m_BlueprintConnectionModified = false;

    typename OutputDataType::Pointer newOutput = this->m_Overlord->GetInitializedOutput( outputName );
    this->m_OutputConnectionModified           = true;

    Superclass::SetOutput( outputName, newOutput );

    return newOutput;
  }
}

template< typename ComponentTypeList >
template< typename ReturnType >
ReturnType *
SuperElastixFilter< ComponentTypeList >
::GetOutput( const DataObjectIdentifierType & outputName )
{
  OutputDataType * output = Superclass::GetOutput( outputName );
  if( output != nullptr ) // if an output already exists, return it
  {
    ReturnType * returnOutput = dynamic_cast< ReturnType * >( output );
    if( returnOutput != nullptr ) // if it is of the same type as requested before
    {
      return returnOutput;
    }
    itkExceptionMacro( << "Output " "" << outputName << "" " was requested before, but the ReturnTypes do not match" )
  }
  // Purposely not checking the outputName, but just create the requested&named data object in the filter.
  // When connecting the Sinks the selxFilter names and data types are checked.
  typename ReturnType::Pointer newOutput = ReturnType::New();

  Superclass::SetOutput( outputName, newOutput );

  this->m_OutputConnectionModified = true;
  return newOutput;
}


template< typename ComponentTypeList >
void
SuperElastixFilter< ComponentTypeList >
::Update( void )
{
  //this->SetPrimaryOutput()
  this->GenerateOutputInformation();
  this->GenerateData();
}
} // namespace elx

#endif // selxSuperElastixFilter_hxx
