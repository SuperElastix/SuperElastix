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
#include "selxNetworkBuilder.h"

#include "selxRegisterComponentFactoriesByTypeList.h"
#include "selxDefaultComponents.h"

namespace selx
{
/**
 * ********************* Constructor *********************
 */

SuperElastixFilter
::SuperElastixFilter(void) : SuperElastixFilter(true)
{
  // The default constructor registers the default components.
  //RegisterFactoriesByTypeList< DefaultComponents >::Register();
  m_NetworkBuilder = std::make_unique<NetworkBuilder<DefaultComponents>>();
} // end Constructor

// 
SuperElastixFilter
::SuperElastixFilter(bool InitializeEmptyComponentList) :
m_InputConnectionModified(true),
m_OutputConnectionModified(true),
m_BlueprintConnectionModified(true),
m_IsConnected(false),
m_AllUniqueComponents(false)
{
   // Disable "Primary" as required input
  // TODO: Blueprint should become primary
  this->SetRequiredInputNames({});
} // end Constructor


/**
* ********************* GenerateOutputInformation *********************
*/

void
SuperElastixFilter
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
    if (!this->m_Blueprint)
    {
      itkExceptionMacro(<< "Setting a Blueprint is required first.")
    }

    this->m_NetworkBuilder->AddBlueprint(this->m_Blueprint->Get());
    this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
  if (this->m_BlueprintConnectionModified == true)
  {
//TODO: 
  }

  if( ( m_InputConnectionModified == true ) || ( this->m_BlueprintConnectionModified == true ) )
  {
    auto                             usedInputs = this->GetInputNames();
    NetworkBuilderBase::SourceInterfaceMapType sources = this->m_NetworkBuilder->GetSourceInterfaces();
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
    NetworkBuilderBase::SinkInterfaceMapType sinks = this->m_NetworkBuilder->GetSinkInterfaces();
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

  if (this->m_AllUniqueComponents == false) // by setting inputs and outputs, settings could be derived to uniquely select the other components
  {
    this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
  }

  if (this->m_AllUniqueComponents  && !this->m_IsConnected)
  {
    this->m_IsConnected = this->m_NetworkBuilder->ConnectComponents();
    std::cout << "Connecting Components: " << (this->m_IsConnected ? "succeeded" : "failed") << std::endl << std::endl;
  }

  if( ( m_OutputConnectionModified == true ) || ( this->m_BlueprintConnectionModified == true ) )
  {
    NetworkBuilderBase::SinkInterfaceMapType sinks = this->m_NetworkBuilder->GetSinkInterfaces();
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

void
SuperElastixFilter
::GenerateData( void )
{
  std::cout << "Executing Network:" << std::endl;
  

  auto fullyConfiguredNetwork  = this->m_NetworkBuilder->GetRealizedNetwork();
  // delete the networkbuilder
  // this->m_NetworkBuilder = nullptr; 

  // This calls controller components that take over the control flow if the itk pipeline is broken.
  fullyConfiguredNetwork.Execute();

  // Connect the itk pipeline.
  auto outputObjectsMap = fullyConfiguredNetwork.GetOutputObjectsMap();
  for (const auto & nameAndObject : outputObjectsMap)
  {
    // Here we force all output to be updated.
    // TODO: it might be desirable to leave parts of the mini pipeline 'outdated' for memory/speed reasons and only update if requested downsteam.
    nameAndObject.second->Update();
    this->GetOutput(nameAndObject.first)->Graft(nameAndObject.second);
  }
}


SuperElastixFilter::AnyFileReaderType::Pointer
SuperElastixFilter
::GetInputFileReader(const DataObjectIdentifierType & inputName)
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the NetworkBuilder.
  // This is not like the itk pipeline philosophy
  if (!this->m_NetworkBuilder)
  {
    if (!this->m_Blueprint)
    {
      itkExceptionMacro(<< "Setting a Blueprint is required first.")
    }
    this->m_NetworkBuilder->AddBlueprint(this->m_Blueprint->Get());
    this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
  }
  if (!this->m_AllUniqueComponents)
  {
    itkExceptionMacro(<< "Blueprint was not sufficiently specified to build a network.")
  }
  return this->m_NetworkBuilder->GetInputFileReader( inputName );
}


SuperElastixFilter::AnyFileWriterType::Pointer
SuperElastixFilter
::GetOutputFileWriter( const DataObjectIdentifierType & outputName )
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the NetworkBuilder.
  // This is not like the itk pipeline philosophy
  if (!this->m_NetworkBuilder)
  {
    if (!this->m_Blueprint)
    {
      itkExceptionMacro(<< "Setting a Blueprint is required first.")
    }

    this->m_NetworkBuilder->AddBlueprint(this->m_Blueprint->Get());
    this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
  }
  if (!this->m_AllUniqueComponents)
  {
    itkExceptionMacro(<< "Blueprint was not sufficiently specified to build a network.")
  }

  return this->m_NetworkBuilder->GetOutputFileWriter( outputName );
}


void
SuperElastixFilter
::SetInput( const DataObjectIdentifierType & inputName, itk::DataObject * input )
{
  Superclass::SetInput( inputName, input );
  this->m_InputConnectionModified = true;
}


SuperElastixFilter::OutputDataType
* SuperElastixFilter
::GetOutput( const DataObjectIdentifierType &outputName )
{
  OutputDataType * output = Superclass::GetOutput( outputName );
  if( output != nullptr ) // if an output already exists, return it
  {
    return output;
  }
  else  // otherwise ask the sink component to initialize an output of the right type (the sink knows what type that is).
  {
    if (!this->m_NetworkBuilder)
    {
      if (!this->m_Blueprint) // to ask the sink it must be configured by a blueprint.
      {
        itkExceptionMacro(<< "Setting a Blueprint is required first.")
      }

      this->m_NetworkBuilder->AddBlueprint(this->m_Blueprint->Get());
      this->m_AllUniqueComponents = this->m_NetworkBuilder->Configure();
      this->m_BlueprintConnectionModified = false;
    }
    OutputDataType::Pointer newOutput = this->m_NetworkBuilder->GetInitializedOutput( outputName );
    this->m_OutputConnectionModified           = true;

    Superclass::SetOutput( outputName, newOutput );

    return newOutput;
  }
}


void
SuperElastixFilter
::Update( void )
{
  //this->SetPrimaryOutput()
  this->GenerateOutputInformation();
  this->GenerateData();
}
} // namespace elx

#endif // selxSuperElastixFilter_hxx
