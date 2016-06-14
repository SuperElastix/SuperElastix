#ifndef selxSuperElastixFilter_hxx
#define selxSuperElastixFilter_hxx

#include "selxSuperElastixFilter.h"


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
  bool isSuccess(false);
  bool allUniqueComponents;
  this->m_Overlord->ApplyNodeConfiguration();
  std::cout << "Applying Component Settings" << std::endl;
  allUniqueComponents = this->m_Overlord->UpdateSelectors();
  std::cout << "Based on Component Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

  std::cout << "Applying Connection Settings" << std::endl;
  this->m_Overlord->ApplyConnectionConfiguration();
  allUniqueComponents = this->m_Overlord->UpdateSelectors();
  std::cout << "By adding Connection Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

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

  if (allUniqueComponents)
  {
    isSuccess = this->m_Overlord->ConnectComponents();
  }
  std::cout << "Connecting Components: " << (isSuccess ? "succeeded" : "failed") << std::endl;

  // TODO make one "update button" for the overlord
  this->m_Overlord->FindRunRegistration();
  this->m_Overlord->FindAfterRegistration();

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
  //this->SetPrimaryOutput()
  return Superclass::GetOutput(outputName);
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
