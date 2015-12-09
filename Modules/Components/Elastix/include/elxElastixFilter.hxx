#ifndef ElastixComponent_hxx
#define ElastixComponent_hxx

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
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::GenerateData( void )
{
  ElastixMainObjectPointer    transform            = 0;
  DataObjectContainerPointer  fixedImageContainer  = this->m_FixedImageContainer;
  DataObjectContainerPointer  movingImageContainer = this->m_MovingImageContainer;
  DataObjectContainerPointer  fixedMaskContainer   = 0;
  DataObjectContainerPointer  movingMaskContainer  = 0;
  DataObjectContainerPointer  resultImageContainer = 0;

  ParameterMapListType        TransformParameterMapList;

  // Fixed mask (optional)
  if( this->HasInput( "FixedMask" ) )
  {
    fixedMaskContainer                        = DataObjectContainerType::New();                   
    fixedMaskContainer->CreateElementAt( 0 )  = this->GetInput( "FixedMask" );
  }

  // Moving mask (optional)
  if( this->HasInput( "MovingMask" )  )
  {
    movingMaskContainer                       = DataObjectContainerType::New();
    movingMaskContainer->CreateElementAt( 0 ) = this->GetInput( "MovingMask" );
  }

  // Get ParameterMap
  ParameterObjectPointer parameterObject = static_cast< ParameterObject* >( this->GetInput( "ParameterObject" ) );
  ParameterMapListType parameterMapList = parameterObject->GetParameterMapList();

  // Emulate command line parameters. There must be an "-out", this is checked later in code
  ArgumentMapType argumentMap;
  argumentMap.insert( ArgumentMapEntryType( std::string( "-out" ), std::string( "output_path_not_set" ) ) );

  // Direction Cosines
  FlatDirectionCosinesType fixedImageOriginalDirection;

  // Setup xout
  if( elx::xoutSetup( this->GetLogToFile().c_str(), this->GetLogToFile() != std::string(), this->GetLogToConsole() ) )
  {
    itkExceptionMacro( "ERROR while setting up xout." );
  }

  // Run the (possibly multiple) registration(s)
  unsigned int isError;
  for( unsigned int i = 0; i < parameterMapList.size(); ++i )
  {
    // Create another instance of ElastixMain 
    ElastixMainPointer elastix = ElastixMainType::New();

    // Set stuff we get from a previous registration 
    elastix->SetInitialTransform( transform );
    elastix->SetFixedImageContainer( fixedImageContainer );
    elastix->SetMovingImageContainer( movingImageContainer );
    elastix->SetFixedMaskContainer( fixedMaskContainer );
    elastix->SetMovingMaskContainer( movingMaskContainer );
    elastix->SetResultImageContainer( resultImageContainer );
    elastix->SetOriginalFixedImageDirectionFlat( fixedImageOriginalDirection );

    // Set the current elastix-level
    elastix->SetElastixLevel( i );
    elastix->SetTotalNumberOfElastixLevels( 1 );

    // ITK pipe-line mechanism need a result image
    parameterMapList[ i ][ "WriteResultImage"] = ParameterValuesType( 1, std::string( "true" ) );

    // Start registration
    isError = elastix->Run( argumentMap, parameterMapList[ i ] );

    // Assert success 
    if( isError != 0 )
    {
      itkExceptionMacro( "Errors occured" );
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
  this->SetOutput( "TransformParametersObject", static_cast< itk::DataObject* >( TransformParameters ) );

  // Clean up
  transform            = 0;
  fixedImageContainer  = 0;
  movingImageContainer = 0;
  fixedMaskContainer   = 0;
  movingMaskContainer  = 0;
  resultImageContainer = 0;

  // Close the modules
  ElastixMainType::UnloadComponents();

}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedImage( FixedImagePointer fixedImage )
{
  if( this->m_FixedImageContainer->Size() > 0 )
  {
    // Free images that has already been given
    this->m_FixedImageContainer = 0;
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
  if( this->m_MovingImageContainer->Size() > 0 )
  {
    // Free images that has already been given
    this->m_MovingImageContainer = 0;
  }

  // Input for elastix
  this->m_MovingImageContainer->CreateElementAt( 0 ) = static_cast< itk::DataObject* >( movingImage ) ;

  // Primary input for ITK pipeline
  this->SetInput( DataObjectIdentifierType( "MovingImage" ), this->m_MovingImageContainer->ElementAt( 0 ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingImage( DataObjectContainerPointer movingImages )
{
  // Input for elastix
  this->m_MovingImageContainer = movingImages;

  // Primary input for ITK pipeline
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
  return static_cast< ParameterObject* >( itk::ProcessObject::GetOutput( DataObjectIdentifierType( "TransformParametersObject" ) ) );
}

} // namespace selx

#endif // ElastixComponent_hxx