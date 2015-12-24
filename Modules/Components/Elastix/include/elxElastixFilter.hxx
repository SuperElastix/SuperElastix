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
  this->m_LogFileName = std::string();

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
  DataObjectContainerPointer  fixedImageContainer  = DataObjectContainerType::New();
  DataObjectContainerPointer  movingImageContainer = DataObjectContainerType::New();
  DataObjectContainerPointer  fixedMaskContainer   = 0;
  DataObjectContainerPointer  movingMaskContainer  = 0;
  DataObjectContainerPointer  resultImageContainer = 0;
  ParameterMapListType        TransformParameterMapList;
  FlatDirectionCosinesType    fixedImageOriginalDirection;

  // Split inputs into separate containers
  InputNameArrayType inputNames = this->GetInputNames();
  for( unsigned int i = 0; i < inputNames.size(); ++i )
  {
    if( this->IsInputType( "FixedImage", inputNames[ i ] ) )
    {
      fixedImageContainer->push_back( this->GetInput( inputNames[ i ] ) );
      continue;
    }

    if( this->IsInputType( "MovingImage", inputNames[ i ] ) )
    {
      movingImageContainer->push_back( this->GetInput( inputNames[ i ] ) );
      continue;
    }

    if( this->IsInputType( "FixedMask", inputNames[ i ] ) )
    {
      // When the first mask is set, we initialize the container, which is zero by default.
      // The user can unuse fixed masks with RemoveFixedMask() that resets the pointer to zero
      if( !fixedMaskContainer )
      {
        fixedMaskContainer = DataObjectContainerType::New();                   
      }

      fixedMaskContainer->push_back( this->GetInput( inputNames[ i ] ) );
      continue;
    }

    if( this->IsInputType( "Movingmask", inputNames[ i ] ) )
    {
      // When the first mask is set, we initialize the container, which is zero by default.
      // The user can unuse moving masks with RemoveMovingMask() that resets the pointer to zero
      if( !movingMaskContainer )
      {
        movingMaskContainer = DataObjectContainerType::New();
      }

      movingMaskContainer->push_back( this->GetInput( inputNames[ i ] ) );
    }
  }

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

    if( !itksys::SystemTools::FileExists( this->GetOutputDirectory() ) )
    {
      itkExceptionMacro( "Output directory \"" << this->GetOutputDirectory() << "\" does not exist." )
    }

    if( this->GetOutputDirectory().back() != '/' || this->GetOutputDirectory().back() != '\\' )
    {
      this->SetOutputDirectory( this->GetOutputDirectory() + "/" );
    }

    if( this->GetLogFileName().empty() )
    {
      logFileName = this->GetOutputDirectory() + "transformix.log";
    }
    else
    {
      logFileName = this->GetOutputDirectory() + this->GetLogFileName();
    }
  }

  if( elx::xoutSetup( logFileName.c_str(), this->GetLogToFile(), this->GetLogToConsole() ) )
  {
    itkExceptionMacro( "ERROR while setting up xout" );
  }

  // Get ParameterMap
  ParameterObjectConstPointer parameterObject = static_cast< const ParameterObject* >( this->GetInput( "ParameterObject" ) );
  ParameterMapListType parameterMapList = parameterObject->GetParameterMapList();

  if( parameterMapList.size() == 0 )
  {
    itkExceptionMacro( "Parameter object contains an empty parameter map list." );
  }

  // Elastix must always write result image to guarantee that ITK pipeline is in a consistent state
  parameterMapList[ parameterMapList.size()-1 ][ "WriteResultImage" ] = ParameterVectorType( 1, "true" );

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
      itkExceptionMacro( << "Uncought errors occurred during registration." );
    }

    // Get the transform, the fixedImage and the movingImage
    // in order to put it in the next registration
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
    this->GraftOutput( "ResultImage", resultImageContainer->ElementAt( 0 ) );
  }

  // Save parameter map
  ParameterObject::Pointer TransformParameterObject = ParameterObject::New();
  TransformParameterObject->SetParameterMapList( TransformParameterMapList );
  this->SetOutput( "TransformParameterObject", static_cast< itk::DataObject* >( TransformParameterObject ) );

  // Close the modules
  ElastixMainType::UnloadComponents();
}

template< typename TFixedImage, typename TMovingImage >
typename ElastixFilter< TFixedImage, TMovingImage >::FixedImagePointer
ElastixFilter< TFixedImage, TMovingImage >
::GetOutput( void )
{
  // TODO: This update should not be needed but the ElastixFilterTest.UpdateOnGetOutput fails without it
  this->Update();

  return static_cast< TFixedImage* >( itk::ProcessObject::GetPrimaryOutput() );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetParameterObject( ParameterObjectPointer parameterObject )
{
  this->SetInput( "ParameterObject", static_cast< itk::DataObject* >( parameterObject ) );
}

template< typename TFixedImage, typename TMovingImage >
typename selx::ElastixFilter< TFixedImage, TMovingImage >::ParameterObjectPointer
ElastixFilter< TFixedImage, TMovingImage >
::GetTransformParameters( void )
{
  // Make sure the transform parameters are up to date
  this->Update();

  return static_cast< ParameterObject* >( itk::ProcessObject::GetOutput( "TransformParameterObject" ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedImage( FixedImagePointer fixedImage )
{
  // Free references to fixed images that has already been set
  this->RemoveInputType( "FixedImage" );

  this->SetInput( "FixedImage", static_cast< itk::DataObject* >( fixedImage ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedImage( DataObjectContainerPointer fixedImages )
{
  if( fixedImages->Size() == 0 )
  {
    itkExceptionMacro( "Cannot set fixed images from empty container.")
  }

  // Free references to fixed images that has already been set
  this->RemoveInputType( "FixedImage" );

  // The ITK filter requires a "FixedImage" named input. The first image 
  // will thus be named  "FixedImage" while The rest of the images will 
  // be appended to the input container suffixed with _1, _2, etc. 
  // The common prefix allows us to read out only the fixed images 
  // for elastix moving image container at a later stage
  DataObjectContainerIterator fixedImageIterator = fixedImages->Begin();
  this->SetInput( "FixedImage", fixedImageIterator->Value() );
  ++fixedImageIterator;

  while( fixedImageIterator != fixedImages->End() )
  {
    this->AddInputAutoIncrementName( "FixedImage", fixedImageIterator->Value() );
    ++fixedImageIterator;
  }
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::AddFixedImage( FixedImagePointer fixedImage )
{
  if( !this->HasInput( "FixedImage") )
  {
    this->SetFixedImage( fixedImage );
  }
  else
  {
    this->AddInputAutoIncrementName( "FixedImage", static_cast< itk::DataObject* >( fixedImage ) );
  }
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingImage( MovingImagePointer movingImage )
{
  // Free references to moving images that has already been set
  this->RemoveInputType( "MovingImage" );

  this->SetInput( "MovingImage", static_cast< itk::DataObject* >( movingImage ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingImage( DataObjectContainerPointer movingImages )
{
  if( movingImages->Size() == 0 )
  {
    itkExceptionMacro( "Cannot set moving images from empty container.")
  }

  // Free references to fixed images that has already been set
  this->RemoveInputType( "MovingImage" );

  // The ITK filter requires a "MovingImage" named input. The first image 
  // will thus be named  "MovingImage" while The rest of the images will 
  // be appended to the input container suffixed with _1, _2, etc. 
  // The common prefix allows us to read out only the moving images 
  // for elastix moving image container at a later stage
  DataObjectContainerIterator movingImageIterator = movingImages->Begin();
  this->SetInput( "MovingImage", movingImageIterator->Value() );
  ++movingImageIterator;

  while( movingImageIterator != movingImages->End() )
  {
    this->AddInputAutoIncrementName( "MovingImage", static_cast< itk::DataObject* >( movingImageIterator->Value() ) );
    ++movingImageIterator;
  }
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::AddMovingImage( MovingImagePointer movingImage )
{
  if( !this->HasInput( "MovingImage") )
  {
    this->SetMovingImage( movingImage );
  }
  else
  {
    this->AddInputAutoIncrementName( "MovingImage", static_cast< itk::DataObject* >( movingImage ) );
  }
}


template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedMask( FixedImagePointer fixedMask )
{
  // Free references to fixed masks that has already been set
  this->RemoveInputType( "FixedMask" );

  this->SetInput( "FixedMask", static_cast< itk::DataObject* >( fixedMask) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetFixedMask( DataObjectContainerPointer fixedMasks )
{
  if( fixedMasks->Size() == 0 )
  {
    itkExceptionMacro( "Cannot set fixed masks from empty container.")
  }

  // Free references to fixed images that has already been set
  this->RemoveInputType( "FixedMask" );

  DataObjectContainerIterator fixedMaskIterator = fixedMasks->Begin();
  while( fixedMaskIterator != fixedMasks->End() )
  {
    this->AddInputAutoIncrementName( "FixedMask", fixedMaskIterator->Value() );
    ++fixedMaskIterator;
  }
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::AddFixedMask( FixedImagePointer fixedMask )
{
  this->AddInputAutoIncrementName( "FixedMask", static_cast< itk::DataObject* >( fixedMask ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::RemoveFixedMask( void )
{
  this->RemoveInputType( "FixedMask" );
  this->m_FixedMaskContainer = 0;
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingMask( MovingImagePointer movingMask )
{
  // Free references to moving masks that has already been set
  this->RemoveInputType( "MovingMask" );

  this->SetInput( "MovingMask", static_cast< itk::DataObject* >( movingMask ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::SetMovingMask( DataObjectContainerPointer movingMasks )
{
  if( movingMasks->Size() == 0 )
  {
    itkExceptionMacro( "Cannot set moving images from empty container.")
  }

  // Free references to fixed images that has already been set
  this->RemoveInputType( "MovingMask" );

  DataObjectContainerIterator movingMaskIterator = movingMasks->Begin();
  while( movingMaskIterator != movingMasks->End() )
  {
    this->AddInputAutoIncrementName( "MovingMask", static_cast< itk::DataObject* >( movingMaskIterator->Value() ) );
    ++movingMaskIterator;
  }
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::AddMovingMask( MovingImagePointer movingMask )
{
  this->AddInputAutoIncrementName( "MovingMask", static_cast< itk::DataObject* >( movingMask ) );
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::RemoveMovingMask( void )
{
  this->RemoveInputType( "MovingMask" );
  this->m_MovingMaskContainer = 0;
}

/*
 * Adds a named input to the first null position in the input list
 * and expands the list memory if necessary
 */
template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::AddInputAutoIncrementName( DataObjectIdentifierType inputName, itk::DataObject* input )
{
  for ( unsigned idx = 0; idx < this->GetNumberOfIndexedInputs(); ++idx )
  {
    if ( !this->GetInput( idx ) )
    {
      // append e.g. '_2' to inputName 
      inputName += this->MakeNameFromInputIndex( idx );
      this->SetInput( inputName, input );
      return;
    }
  }

  inputName += this->MakeNameFromInputIndex( this->GetNumberOfIndexedInputs() );
  this->SetInput( inputName, input );
  return;
}

template< typename TFixedImage, typename TMovingImage >
bool
ElastixFilter< TFixedImage, TMovingImage >
::IsInputType( DataObjectIdentifierType inputType, DataObjectIdentifierType inputName )
{
  return std::strncmp( inputType.c_str(), inputName.c_str(), inputType.size() ) == 0;
}

template< typename TFixedImage, typename TMovingImage >
void
ElastixFilter< TFixedImage, TMovingImage >
::RemoveInputType( DataObjectIdentifierType inputType )
{
  // Free references to fixed images that has already been set
  InputNameArrayType inputNames = this->GetInputNames();
  for( unsigned int i = 0; i < inputNames.size(); ++i )
  {
    if ( this->IsInputType( inputType, inputNames[ i ] ) )
    {
      this->RemoveInput( inputNames[ i ] );
    }
  }
}

} // namespace selx

#endif // ElastixFilter_hxx