#ifndef ElastixComponent_hxx
#define ElastixComponent_hxx

namespace selx {

template< typename TOutputImage >
ElastixComponent< TOutputImage >
::ElastixComponent() {
  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );

  this->SetPrimaryInput(this->GetInput("FixedImage"));

  // TODO: Handle these special elastix parameters in a more ITK-esque way
  // For now, don't expose them but just log to console and avoid writing to disk
  this->m_OutputFolder = std::string();
  this->m_LogToConsole = true;
  this->m_ParameterMapList = ParameterMapListType();
}

template< typename TOutputImage >
void
ElastixComponent< TOutputImage >
::GenerateData( void )
{
  // Assert at least one parameter map is set
  if( this->m_ParameterMapList.size() == 0u )
  {
    itkExceptionMacro( << "No parameter map. Use SetParameterMap() to supply parameter map." );
  }

  // Get masks (optional)
  itk::DataObject::Pointer fixedMask = 0;
  if( !( this->GetInput( "FixedMask" ) == ITK_NULLPTR ) )
  {
    fixedMask = this->GetInput( "FixedMask" );
  }

  itk::DataObject::Pointer movingMask = 0;
  if( !( this->GetInput( "MovingMask" ) == ITK_NULLPTR ) )
  {
    movingMask = this->GetInput( "MovingMask" );
  }

  // Do the (possibly multiple) registrations
  ElastixType elastix = ElastixType();
  int isError = 0;
  try
  {
    isError = elastix.RegisterImages(
      this->ProcessObject::GetInput( "FixedImage" ),
      this->ProcessObject::GetInput( "MovingImage" ),
      this->m_ParameterMapList,
      this->m_OutputFolder,
      this->m_OutputFolder != std::string(),
      this->m_LogToConsole,
      fixedMask,
      movingMask
    );
  }
  catch( itk::ExceptionObject &e )
  {
    itkExceptionMacro( << "Errors occured during registration: " << e.what() );
  }

  if( isError == -2 )
  {
    itkExceptionMacro( << "Errors occured during registration: Output directory does not exist." );
  }

  if( isError != 0 )
  {
    itkExceptionMacro( << "Errors occured during registration." );
  }

  if( elastix.GetResultImage().IsNotNull() )
  {
    this->ProcessObject::SetNthOutput( 0, elastix.GetResultImage().GetPointer() );
  }
}

} // namespace selx

#endif // ElastixComponent_hxx