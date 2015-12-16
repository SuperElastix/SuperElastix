#ifndef ElastixFilter_hxx
#define ElastixFilter_hxx

namespace selx {

template< typename TFixedImage, typename TMovingImage >
ElastixFilter< TFixedImage, TMovingImage >
::ElastixFilter( void )
{
  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );
  this->AddRequiredInputName( "ParameterObject");

  this->SetPrimaryInputName( "FixedImage" );
  this->SetPrimaryOutputName( "ResultImage" );

  this->m_FixedImageContainer = DataObjectContainerType::New();
  this->m_MovingImageContainer = DataObjectContainerType::New();

  this->m_FixedMeshFileName = std::string();
  this->m_MovingMeshFileName = std::string();

  this->m_OutputDirectory = std::string();
  this->LogToConsoleOff();
  this->LogToFileOff();
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::GenerateData( void )
{
  // Initialize variables here so they don't go out of scope between iterations of the main loop
  ElastixMainObjectPointer    transform            = 0;
  DataObjectContainerPointer  fixedImageContainer  = this->m_FixedImageContainer;
  DataObjectContainerPointer  movingImageContainer = this->m_MovingImageContainer;
  DataObjectContainerPointer  fixedMaskContainer   = 0;
  DataObjectContainerPointer  movingMaskContainer  = 0;
  DataObjectContainerPointer  resultImageContainer = 0;
  ParameterMapListType        TransformParameterMapList;
  FlatDirectionCosinesType    fixedImageOriginalDirection;

  // Fixed mask (optional)
  if( this->HasInput( "FixedMask" ) )
  {
    fixedMaskContainer = DataObjectContainerType::New();                   
    fixedMaskContainer->CreateElementAt( 0 )  = this->GetInput( "FixedMask" );
  }

  // Moving mask (optional)
  if( this->HasInput( "MovingMask" )  )
  {
    movingMaskContainer = DataObjectContainerType::New();
    movingMaskContainer->CreateElementAt( 0 ) = this->GetInput( "MovingMask" );
  }

  // There must be an "-out", this is checked later in the code
  ArgumentMapType argumentMap;
  if( this->GetOutputDirectory().empty() ) {
    // There must be an "-out", this is checked later in the code
    argumentMap.insert( ArgumentMapEntryType( "-out", "output_path_not_set" ) );
  }
  else
  {
    argumentMap.insert( ArgumentMapEntryType( "-out", this->GetOutputDirectory() ) );
  }

  // Fixed mesh (optional)
  if( !this->m_FixedMeshFileName.empty() )
  {
    argumentMap.insert( ArgumentMapEntryType( "-fp", std::string( this->m_FixedMeshFileName ) ) );
  }

  // Moving mesh (optional)
  if( !this->m_MovingMeshFileName.empty() )
  {
    argumentMap.insert( ArgumentMapEntryType( "-mp", std::string( this->m_MovingMeshFileName ) ) );
  }

  // Setup xout
  std::string logFileName;
  if( this->GetLogToFile() )
  {
    if( this->GetOutputDirectory().empty() )
    {
      itkExceptionMacro( "LogToFileOn() requires an output directory to be specified. Use SetOutputDirectory().")
    }
    logFileName = this->GetOutputDirectory() + "transformix.log";
  }

  if( elx::xoutSetup( logFileName.c_str(), this->GetLogToFile(), this->GetLogToConsole() ) )
  {
    // TODO: The following exception thrown if two different filters are initialized by the same process
    itkExceptionMacro( "ERROR while setting up xout" );
  }


  // Get ParameterMap
  ParameterObjectConstPointer parameterObject = static_cast< const ParameterObject* >( this->GetInput( "ParameterObject" ) );
  ParameterMapListType parameterMapList = parameterObject->GetParameterMapList();

  // Run the (possibly multiple) registration(s)
  for( unsigned int i = 0; i < parameterMapList.size(); ++i )
  {
    // Create another instance of ElastixMain 
    ElastixMainPointer elastix = ElastixMainType::New();

    // Set the current elastix-level
    elastix->SetElastixLevel( i );
    elastix->SetTotalNumberOfElastixLevels( parameterMapList.size() );

    // Set stuff we get from a previous registration 
    elastix->SetInitialTransform( transform );
    elastix->SetFixedImageContainer( fixedImageContainer );
    elastix->SetMovingImageContainer( movingImageContainer );
    elastix->SetFixedMaskContainer( fixedMaskContainer );
    elastix->SetMovingMaskContainer( movingMaskContainer );
    elastix->SetResultImageContainer( resultImageContainer );
    elastix->SetOriginalFixedImageDirectionFlat( fixedImageOriginalDirection );

    // Start registration
    unsigned int isError = 0;
    try
    {
      unsigned int isError = elastix->Run( argumentMap, parameterMapList[ i ] );
    }
    catch( itk::ExceptionObject &e )
    {
      itkExceptionMacro( << "Errors occurred during registration: " << e.what() );
    }

    if( isError == -2 )
    {
      itkExceptionMacro( << "Errors occurred during registration: Output directory does not exist." );
    }

    if( isError != 0 )
    {
      std::cout << std::endl << isError << std::endl;
      itkExceptionMacro( << "Uncought errors occurred during registration." );
    }

    // Get the transform, the fixedImage and the movingImage
    // in order to put it in the next registration.
    transform                   = elastix->GetFinalTransform();
    fixedImageContainer         = elastix->GetFixedImageContainer();
    movingImageContainer        = elastix->GetMovingImageContainer();
    fixedMaskContainer          = elastix->GetFixedMaskContainer();
    movingMaskContainer         = elastix->GetMovingMaskContainer();
    resultImageContainer        = elastix->GetResultImageContainer();
    fixedImageOriginalDirection = elastix->GetOriginalFixedImageDirectionFlat();

    TransformParameterMapList.push_back( elastix->GetTransformParametersMap() );

    // Set initial transform to an index number instead of a parameter filename
    if( i > 0 )
    {
      std::stringstream index;
      index << ( i - 1 );
      TransformParameterMapList[ i ][ "InitialTransformParametersFileName" ][ 0 ] = index.str();
    }
  } // End loop over registrations

  // Save result image
  if( resultImageContainer.IsNotNull() && resultImageContainer->Size() > 0 )
  {
    this->SetOutput( "ResultImage", resultImageContainer->ElementAt( 0 ) );
  }

  // Save parameter map
  ParameterObject::Pointer TransformParameters = ParameterObject::New();
  TransformParameters->SetParameterMapList( TransformParameterMapList );
  this->SetOutput( "TransformParameterObject", static_cast< itk::DataObject* >( TransformParameters ) );

  // Clean up
  transform            = ITK_NULLPTR;
  fixedImageContainer  = ITK_NULLPTR;
  movingImageContainer = ITK_NULLPTR;
  fixedMaskContainer   = ITK_NULLPTR;
  movingMaskContainer  = ITK_NULLPTR;
  resultImageContainer = ITK_NULLPTR;

  // Close the modules
  ElastixMainType::UnloadComponents();

}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedImage( FixedImagePointer fixedImage )
{
  // Free references to fixed images that has already been set
  if( this->m_FixedImageContainer->Size() > 0 )
  {
    this->m_FixedImageContainer = ITK_NULLPTR;
  }

  // Input for elastix
  this->m_FixedImageContainer->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( fixedImage ) ;

  // Primary input for ITK pipeline
  this->SetInput( DataObjectIdentifierType( "FixedImage" ), this->m_FixedImageContainer->ElementAt( 0 ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedImage( DataObjectContainerPointer fixedImages )
{
  // Input for elastix
  this->m_FixedImageContainer = fixedImages;

  // Primary input for ITK pipeline
  this->SetInput( DataObjectIdentifierType( "FixedImage" ), this->m_FixedImageContainer->ElementAt( 0 ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingImage( MovingImagePointer movingImage )
{
  // Free references to moving images that has already been set
  if( this->m_MovingImageContainer->Size() > 0 )
  {
    
    this->m_MovingImageContainer = ITK_NULLPTR;
  }

  // Input for elastix
  this->m_MovingImageContainer->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( movingImage ) ;

  // Input for ITK pipeline
  this->SetInput( DataObjectIdentifierType( "MovingImage" ), this->m_MovingImageContainer->ElementAt( 0 ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingImage( DataObjectContainerPointer movingImages )
{
  // Input for elastix
  this->m_MovingImageContainer = movingImages;

  // Input for ITK pipeline
  this->SetInput( DataObjectIdentifierType( "MovingImage" ), this->m_MovingImageContainer->ElementAt( 0 ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetParameterObject( ParameterObjectPointer parameterObject )
{
  this->SetInput( DataObjectIdentifierType( "ParameterObject" ), static_cast< itk::DataObject* >( parameterObject ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedMask( FixedImagePointer fixedMask )
{
  this->SetInput( DataObjectIdentifierType( "FixedMask" ), static_cast< itk::DataObject* >( fixedMask ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingMask( MovingImagePointer movingMask )
{
  this->SetInput( DataObjectIdentifierType( "MovingMask" ), static_cast< itk::DataObject* >( movingMask ) );
}

template< typename TFixedImage, typename TMovingImage >
typename selx::ElastixFilter< TFixedImage, TMovingImage >::ParameterObjectPointer
ElastixFilter< TFixedImage, TMovingImage >
::GetTransformParameters( void )
{
  return static_cast< ParameterObject* >( itk::ProcessObject::GetOutput( DataObjectIdentifierType( "TransformParameterObject" ) ) );
}

} // namespace selx

#endif // ElastixFilter_hxx