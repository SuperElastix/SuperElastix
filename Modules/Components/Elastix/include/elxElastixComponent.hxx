#ifndef ElastixComponent_hxx
#define ElastixComponent_hxx

namespace selx {

template< typename TOutputImage >
ElastixComponent< TOutputImage >
::ElastixComponent()
{
  m_Elastix = ElastixType();

  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );

  this->SetPrimaryInput( this->GetInput( "FixedImage" ) );
  this->SetPrimaryOutput( m_Elastix.GetResultImage().GetPointer() );

  // TODO: Handle these special elastix parameters in a more ITK-esque way
  // For now, don't expose them but just log to console and avoid writing to disk
  this->m_OutputFolder = std::string( "" );
  this->m_LogToConsole = true;
}

template< typename TOutputImage >
void
ElastixComponent< TOutputImage >
::GenerateData( void )
{
  itkExceptionMacro( "Running GenerateData" );

  int isError = 1;
  
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
  try
  {
    isError = m_Elastix.RegisterImages(
      this->GetInput( "FixedImage" ),
      this->GetInput( "MovingImage" ),
      this->m_ParameterMapList,
      this->m_OutputFolder,
      this->m_OutputFolder != "",
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
    itkExceptionMacro( << "Errors occured during registration. If you do not see any error message, set LogToConsoleOn() or LogToFolder(\"path/to/folder\") to view elastix output." );
  }

  if( m_Elastix.GetResultImage().IsNotNull() )
  {
    this->SetOutput( "OutputImage", m_Elastix.GetResultImage().GetPointer() );
  }
  else
  {
    itkExceptionMacro( << "Could not read result image." );
  }
}

} // namespace selx

#endif // ElastixComponent_hxx