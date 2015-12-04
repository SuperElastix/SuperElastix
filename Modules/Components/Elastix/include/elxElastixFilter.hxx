#ifndef ElastixComponent_hxx
#define ElastixComponent_hxx

namespace selx {

template< typename TFixedImage, typename TMovingImage, typename TOutputImage >
ElastixFilter< TFixedImage, TMovingImage, TOutputImage >
::ElastixFilter( void )
{
  this->AddRequiredInputName( "FixedImage" );
  this->AddRequiredInputName( "MovingImage" );
  this->AddRequiredInputName( "ParameterObject");

  this->SetPrimaryInputName( "FixedImage" );
  this->SetPrimaryOutputName( "ResultImage" );
}

template< typename TFixedImage, typename TMovingImage, typename TOutputImage >
void
ElastixFilter< TFixedImage, TMovingImage, TOutputImage >
::GenerateData( void )
{
  ElastixMainObjectPointer    transform            = 0;
  DataObjectContainerPointer  fixedImageContainer  = 0;
  DataObjectContainerPointer  movingImageContainer = 0;
  DataObjectContainerPointer  fixedMaskContainer   = 0;
  DataObjectContainerPointer  movingMaskContainer  = 0;
  DataObjectContainerPointer  resultImageContainer = 0;

  ParameterMapListType        TransformParameterMapList;

  // Fixed image
  fixedImageContainer                         = DataObjectContainerType::New();
  fixedImageContainer->CreateElementAt( 0 )   = this->GetInput( "FixedImage" );

  // Moving omage
  movingImageContainer                        = DataObjectContainerType::New();
  movingImageContainer->CreateElementAt( 0 )  = this->GetInput( "MovingImage" );

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
  ParameterMapListType parameterMapList =  parameterObject->GetParameterMapList();

  // Put command line parameters into parameterFileList. There must be an "-out", this is checked later in code
  ArgumentMapType argumentMap;
  argumentMap.insert( ArgumentMapEntryType( std::string( "-out" ), std::string( "output_path_not_set" ) ) );

  // Direction Cosines
  FlatDirectionCosinesType fixedImageOriginalDirection;

  // Setup xout (folder, do file, do cout)
  if( elx::xoutSetup( this->GetLogToFile().c_str(), this->GetLogToFile() != std::string(), this->GetLogToConsole() ) )
  {
    itkExceptionMacro( "ERROR while setting up xout." );
  }

  // Run the (possibly multiple) registration(s)
  unsigned int isError;
  for( unsigned int i = 0; i < parameterMapList.size(); ++i )
  {
    /** Create another instance of ElastixMain. */
    ElastixMainPointer elastix = ElastixMainType::New();

    /** Set stuff we get from a former registration. */
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

    // Start registration
    isError = elastix->Run( argumentMap, parameterMapList[ i ] );

    // Assert success 
    if( isError != 0 )
    {
      itkExceptionMacro( "Errors occured" );
    }

    // Get the transform, the fixedImage and the movingImage
    // in order to put it in the (possibly) next registration.
    transform                   = elastix->GetFinalTransform();
    fixedImageContainer         = elastix->GetFixedImageContainer();
    movingImageContainer        = elastix->GetMovingImageContainer();
    fixedMaskContainer          = elastix->GetFixedMaskContainer();
    movingMaskContainer         = elastix->GetMovingMaskContainer();
    resultImageContainer        = elastix->GetResultImageContainer();
    fixedImageOriginalDirection = elastix->GetOriginalFixedImageDirectionFlat();

    TransformParameterMapList.push_back( elastix->GetTransformParametersMap() );

    // Set initial transform to an index number instead of a parameter filename.
    if( i > 0 )
    {
      std::stringstream index;
      index << ( i - 1 );
      TransformParameterMapList[ i ][ "InitialTransformParametersFileName" ][ 0 ] = index.str();
    }

  } // End loop over registrations

  // TODO: Attach result image and transform parameter map to output
  std::cout << "resultImageContainer.IsNotNull(): " << resultImageContainer.IsNotNull() << std::endl;
  // std::cout << "resultImageContainer->Size(): " << resultImageContainer->Size() << std::endl;
  if( resultImageContainer.IsNotNull() && resultImageContainer->Size() > 0 )
  {
    // this->SetOutput( "ResultImage", resultImageContainer->ElementAt( 0 ) );
    //std::cout << this->GetOutput() << std::endl;
  }

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

template< typename TFixedImage, typename TMovingImage, typename TOutputImage >
void
ElastixFilter< TFixedImage, TMovingImage, TOutputImage >
::SetFixedImage( FixedImagePointer fixedImage )
{
  this->SetInput( itk::ProcessObject::DataObjectIdentifierType( "FixedImage" ), 
                  static_cast< itk::DataObject* >( fixedImage ) );
}

template< typename TFixedImage, typename TMovingImage, typename TOutputImage >
void
ElastixFilter< TFixedImage, TMovingImage, TOutputImage >
::SetMovingImage( MovingImagePointer movingImage )
{
  this->SetInput( itk::ProcessObject::DataObjectIdentifierType( "MovingImage" ), 
                  static_cast< itk::DataObject* >( movingImage ) );
}

template< typename TFixedImage, typename TMovingImage, typename TOutputImage >
void
ElastixFilter< TFixedImage, TMovingImage, TOutputImage >
::SetParameterObject( ParameterObjectPointer parameterObject )
{
  this->SetInput( itk::ProcessObject::DataObjectIdentifierType( "ParameterObject" ), 
                  static_cast< itk::DataObject* >( parameterObject ) );
}

} // namespace selx

#endif // ElastixComponent_hxx