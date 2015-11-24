#ifndef ElastixComponent_hxx
#define ElastixComponent_hxx

namespace selx {

template< typename TOutputImage >
ElastixComponent< TOutputImage >
::ElastixComponent()
{
  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );

  this->SetPrimaryInput( this->GetInput( "FixedImage" ) );

  // TODO: Handle these special elastix parameters in a more ITK-esque way
  // For now, don't expose them but just log to console and avoid writing to disk
  this->m_OutputFolder = std::string( "" );
  this->m_LogToConsole = true;
  this->m_ParameterMapList = ParameterMapListType();
}

template< typename TOutputImage >
void
ElastixComponent< TOutputImage >
::GenerateData( void )
{
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

  itk::DataObject::Pointer fixedImage = this->ProcessObject::GetInput( "FixedImage" );
  itk::DataObject::Pointer movingImage = this->ProcessObject::GetInput( "MovingImage" );
  ParameterMapListType parameterMapList = this->m_ParameterMapList;
  std::string folder = this->m_OutputFolder;
  bool writeToOutputFolder = this->m_OutputFolder != std::string();
  bool writeToConsole = this->m_LogToConsole;

  try
  {
    isError = elastix.RegisterImages( // <-- Why do we get segmentation error even before we enter the function?
      fixedImage,
      movingImage,
      parameterMapList,
      folder,
      writeToOutputFolder,
      writeToConsole
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
  else
  {
    itkExceptionMacro( << "Could not read result image." );
  }
}

} // namespace selx

#endif // ElastixComponent_hxx