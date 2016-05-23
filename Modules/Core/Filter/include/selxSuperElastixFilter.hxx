#ifndef selxSuperElastixFilter_hxx
#define selxSuperElastixFilter_hxx

#include "selxSuperElastixFilter.h"


namespace selx
{

/**
 * ********************* Constructor *********************
 */

template< typename TFixedImage, typename TMovingImage >
SuperElastixFilter< TFixedImage, TMovingImage >
::SuperElastixFilter( void )
{
  this->m_Overlord = Overlord::New();
  
  //Disable "Primary" as required input
  this->SetRequiredInputNames({});

} // end Constructor


/**
* ********************* GenerateOutputInformation *********************
*/


template< typename TFixedImage, typename TMovingImage >
void
SuperElastixFilter< TFixedImage, TMovingImage >
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

  this->m_Overlord->FindAfterRegistration();

  for (const auto & nameAndInterface : sinks)
  {
    nameAndInterface.second->GetMiniPipelineOutput()->UpdateOutputInformation();
  }

}

/**
 * ********************* GenerateData *********************
 */

template< typename TFixedImage, typename TMovingImage >
void
SuperElastixFilter< TFixedImage, TMovingImage >
::GenerateData(void)
{

  this->m_Overlord->Execute();

  Overlord::SinkInterfaceMapType sinks = this->m_Overlord->GetSinkInterfaces();
  for (const auto & nameAndInterface : sinks)
  {
    this->GetOutput(nameAndInterface.first)->Graft(nameAndInterface.second->GetMiniPipelineOutput());
  }

}

template< typename TFixedImage, typename TMovingImage >
void
SuperElastixFilter< TFixedImage, TMovingImage >
::SetInput(const DataObjectIdentifierType& inputName, itk::DataObject* input)
{
  Superclass::SetInput(inputName, input);
  //this->Modified();
}

template< typename TFixedImage, typename TMovingImage >
typename SuperElastixFilter< TFixedImage, TMovingImage >::OutputDataType*
SuperElastixFilter< TFixedImage, TMovingImage >
::GetOutput(const DataObjectIdentifierType& outputName)
{
  //this->SetPrimaryOutput()
  return Superclass::GetOutput(outputName);
}

template< typename TFixedImage, typename TMovingImage >
template<typename ReturnType>
ReturnType*
SuperElastixFilter< TFixedImage, TMovingImage >
::GetOutput(const DataObjectIdentifierType& outputName)
{
  // Purposely not checking the outputName, but just create the requested&named data object in the filter. 
  // When connecting the Sinks the the elxFilter names and data types are checked.
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

template< typename TFixedImage, typename TMovingImage >
void SuperElastixFilter< TFixedImage, TMovingImage >
::ConnectSourceA(itk::DataObject* inputData)
{
  ImageType* fixedImage = dynamic_cast<ImageType*>(inputData);
  this->m_imageFilter->SetInput(fixedImage);
}
template< typename TFixedImage, typename TMovingImage >
void SuperElastixFilter< TFixedImage, TMovingImage >
::ConnectSourceB(itk::DataObject* inputData)
{
  MeshType* fixedMesh = dynamic_cast<MeshType*>(inputData);
  this->m_meshFilter->SetInput(fixedMesh);
}

template< typename TFixedImage, typename TMovingImage >
void SuperElastixFilter< TFixedImage, TMovingImage >
::ConnectPlaceholderSinkA(itk::DataObject* outputPlaceholder)
{
  this->m_imageFilter->GraftOutput(outputPlaceholder);
}
template< typename TFixedImage, typename TMovingImage >
itk::DataObject* SuperElastixFilter< TFixedImage, TMovingImage >
::ConnectDataSinkA()
{
  return this->m_imageFilter->GetOutput();
}

template< typename TFixedImage, typename TMovingImage >
void SuperElastixFilter< TFixedImage, TMovingImage >
::ConnectPlaceholderSinkB(itk::DataObject* outputPlaceholder)
{
  this->m_meshFilter->GraftOutput(outputPlaceholder);
}
template< typename TFixedImage, typename TMovingImage >
itk::DataObject* SuperElastixFilter < TFixedImage, TMovingImage >
::ConnectDataSinkB()
{
  return this->m_meshFilter->GetOutput();
}
} // namespace elx

#endif // selxSuperElastixFilter_hxx
