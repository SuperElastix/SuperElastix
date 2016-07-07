#ifndef selxSuperElastixFilter_hxx
#define selxSuperElastixFilter_hxx

#include "selxSuperElastixFilter.h"
#include "RegisterByTypeList.h"

namespace selx
{

/**
 * ********************* Constructor *********************
 */

  template< typename ComponentTypeList >
  SuperElastixFilter< ComponentTypeList >
::SuperElastixFilter( void )
{
  this->m_Overlord = Overlord::New();

  RegisterFactoriesByTypeList<ComponentTypeList>::Register();

  //Disable "Primary" as required input
  this->SetRequiredInputNames({});

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
  this->m_Overlord->SetBlueprint(this->m_Blueprint);
  bool allUniqueComponents = this->m_Overlord->Configure();
  
  Overlord::SourceInterfaceMapType sources = this->m_Overlord->GetSourceInterfaces();
  for (const auto & nameAndInterface : sources)
  {
    nameAndInterface.second->SetMiniPipelineInput(this->GetInput(nameAndInterface.first));
  }

  Overlord::SinkInterfaceMapType sinks = this->m_Overlord->GetSinkInterfaces();
  for (const auto & nameAndInterface : sinks)
  {
    nameAndInterface.second->SetMiniPipelineOutput(this->GetOutput(nameAndInterface.first));
  }

  bool isSuccess(false);

  if (allUniqueComponents)
  {
    isSuccess = this->m_Overlord->ConnectComponents();
  }
  std::cout << "Connecting Components: " << (isSuccess ? "succeeded" : "failed") << std::endl;

  for (const auto & nameAndInterface : sinks)
  {
    nameAndInterface.second->GetMiniPipelineOutput()->UpdateOutputInformation();
    this->GetOutput(nameAndInterface.first)->Graft(nameAndInterface.second->GetMiniPipelineOutput());
  }

}

/**
 * ********************* GenerateData *********************
 */

template< typename ComponentTypeList >
void
SuperElastixFilter< ComponentTypeList >
::GenerateData(void)
{

  this->m_Overlord->Execute(); // calls updates of sink filters

  Overlord::SinkInterfaceMapType sinks = this->m_Overlord->GetSinkInterfaces();
  for (const auto & nameAndInterface : sinks)
  {
    this->GetOutput(nameAndInterface.first)->Graft(nameAndInterface.second->GetMiniPipelineOutput());
  }

}


template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::AnyFileReaderType::Pointer
SuperElastixFilter< ComponentTypeList >
::GetInputFileReader(const DataObjectIdentifierType& inputName)
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the overlord.
  // This is not like the itk pipeline philosophy
  if (!this->m_Blueprint)
  {
    itkExceptionMacro(<< "Setting a Blueprint is required first.")
  }
  this->m_Overlord->SetBlueprint(this->m_Blueprint);
  this->m_Overlord->Configure();
  
  return this->m_Overlord->GetInputFileReader(inputName);
}

template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::AnyFileWriterType::Pointer
SuperElastixFilter< ComponentTypeList >
::GetOutputFileWriter(const DataObjectIdentifierType& outputName)
{
  //TODO: Before we can get the reader the Blueprint needs to set and applied in the overlord.
  // This is not like the itk pipeline philosophy
  if (!this->m_Blueprint)
  {
    itkExceptionMacro(<< "Setting a Blueprint is required first.")
  }

  this->m_Overlord->SetBlueprint(this->m_Blueprint);
  this->m_Overlord->Configure();

  return this->m_Overlord->GetOutputFileWriter(outputName);
}

template< typename ComponentTypeList>
void
SuperElastixFilter< ComponentTypeList >
::SetInput(const DataObjectIdentifierType& inputName, itk::DataObject* input)
{
  Superclass::SetInput(inputName, input);
  //this->Modified();
}

template< typename ComponentTypeList >
typename SuperElastixFilter< ComponentTypeList >::OutputDataType*
SuperElastixFilter< ComponentTypeList >
::GetOutput(const DataObjectIdentifierType& outputName)
{
  OutputDataType* output = Superclass::GetOutput(outputName);
  if (output != nullptr)
  {
    return output;
  }
  else
  {

    if (!this->m_Blueprint)
    {
      itkExceptionMacro(<< "Setting a Blueprint is required first.")
    }
    this->m_Overlord->SetBlueprint(this->m_Blueprint);
    this->m_Overlord->Configure();

    typename OutputDataType::Pointer newOutput = this->m_Overlord->GetInitializedOutput(outputName);

    Superclass::SetOutput(outputName, newOutput);

    return newOutput;
  }
}

template< typename ComponentTypeList >
template< typename ReturnType >
ReturnType*
SuperElastixFilter< ComponentTypeList >
::GetOutput(const DataObjectIdentifierType& outputName)
{
  // Purposely not checking the outputName, but just create the requested&named data object in the filter. 
  // When connecting the Sinks the selxFilter names and data types are checked.
  typename ReturnType::Pointer newOutput = ReturnType::New();
  
  Superclass::SetOutput(outputName, newOutput);

  //DataObject* baseOutputData = Superclass::GetOutput(outputName);
  //ReturnType* outputData = dynamic_cast<ReturnType*>(baseOutputData);
  //if (!outputData)
  //{
	//      // pointer could not be cast back down
  //  itkExceptionMacro( << "selx::SuperElastixFilter::GetOutput<T>(""" 
  //                     << outputName << """) cannot cast "
  //                     << typeid( baseOutputData ).name() << " to "
  //                     << typeid( ReturnType * ).name() );
	//				   
  //  itkExceptionMacro("OutputData """ << outputName << """ is not of the required type");
  //}
  //return outputData;
  return newOutput;
}

template< typename ComponentTypeList >
void SuperElastixFilter< ComponentTypeList >
::Update(void)
{
  //this->SetPrimaryOutput()
  this->GenerateOutputInformation();
  this->GenerateData();
}

} // namespace elx

#endif // selxSuperElastixFilter_hxx
