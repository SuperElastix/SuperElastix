#ifndef ElastixFilter_h
#define ElastixFilter_h

// ITK
#include "itkImageSource.h"

// Elastix
#include "elxElastixMain.h"

// SuperElastix
#include "elxMacro.h"
#include "elxParameterObject.h"

/**
 * Elastix registration library exposed as an ITK filter.
 * 
 * We have to do some additional bookeeping on input images so that they can
 * be split into separate containers fixed and moving images at a later stage.
 * This is because ITK filters uses a single input container to drive the
 * pipeline while elastix use seperate containers for fixed and moving images.
 * 
 */

namespace selx {

template< typename TFixedImage,
          typename TMovingImage >
class ElastixFilter : public itk::ImageSource< TFixedImage >
{
public:

  elxNewMacro( ElastixFilter, itk::ImageSource );

  typedef elastix::ElastixMain                                ElastixMainType;
  typedef ElastixMainType::Pointer                            ElastixMainPointer;
  typedef std::vector< ElastixMainPointer >                   ElastixMainVectorType;
  typedef ElastixMainType::ObjectPointer                      ElastixMainObjectPointer;

  typedef ElastixMainType::FlatDirectionCosinesType           FlatDirectionCosinesType;

  typedef ElastixMainType::ArgumentMapType                    ArgumentMapType;
  typedef ArgumentMapType::value_type                         ArgumentMapEntryType;

  typedef itk::ProcessObject::DataObjectIdentifierType        DataObjectIdentifierType;
  typedef itk::ProcessObject::DataObjectPointerArraySizeType  DataObjectPointerArraySizeType;
  typedef itk::ProcessObject::NameArray                       InputNameArrayType;

  typedef ElastixMainType::DataObjectContainerType            DataObjectContainerType;
  typedef ElastixMainType::DataObjectContainerPointer         DataObjectContainerPointer;
  typedef DataObjectContainerType::Iterator                   DataObjectContainerIterator;

  typedef ParameterObject::ParameterMapListType               ParameterMapListType;
  typedef ParameterObject::ParameterMapType                   ParameterMapType;
  typedef ParameterObject::ParameterVectorType                ParameterVectorType;
  typedef ParameterObject::Pointer                            ParameterObjectPointer;
  typedef ParameterObject::ConstPointer                       ParameterObjectConstPointer;

  typedef typename TFixedImage::Pointer                       FixedImagePointer;
  typedef typename TMovingImage::Pointer                      MovingImagePointer;

  void SetFixedImage( FixedImagePointer fixedImage );
  void SetFixedImage( DataObjectContainerPointer fixedImages );
  void AddFixedImage( FixedImagePointer fixedImage );

  void SetMovingImage( MovingImagePointer movingImages );
  void SetMovingImage( DataObjectContainerPointer movingImage );
  void AddMovingImage( MovingImagePointer movingImage );

  void SetFixedMask( FixedImagePointer fixedMask );
  void SetMovingMask( MovingImagePointer movingMask );

  void SetParameterObject( ParameterObjectPointer parameterObject );
  ParameterObjectPointer GetTransformParameters( void );

  itkSetMacro( FixedMeshFileName, std::string );
  itkGetConstMacro( FixedMeshFileName, std::string );
  void RemoveFixedMeshFileName( void ) { this->SetFixedMeshFileName( std::string() ); };

  itkSetMacro( MovingMeshFileName, std::string );
  itkGetConstMacro( MovingMeshFileName, std::string );
  void RemoveMovingMeshFileName( void ) { this->SetMovingMeshFileName( std::string() ); };

  itkSetMacro( OutputDirectory, std::string );
  itkGetConstMacro( OutputDirectory, std::string );
  void RemoveOutputDirectory() { this->m_OutputDirectory = std::string(); };

  void SetLogFileName( std::string logFileName )
  {
    this->m_LogFileName = logFileName;
    this->LogToFileOn();
    this->Modified();
  }

  itkGetConstMacro( LogFileName, std::string );

  void RemoveLogFileName( void ) { 
    this->m_LogFileName = std::string();
    this->LogToFileOff();
  };
  
  itkSetMacro( LogToConsole, bool );
  itkGetConstMacro( LogToConsole, bool );
  itkBooleanMacro( LogToConsole );

  itkSetMacro( LogToFile, bool );
  itkGetConstMacro( LogToFile, bool );
  itkBooleanMacro( LogToFile );

  // TODO: Superclass GetOutput() does not trigger an Update() by itself as it should
  TFixedImage* GetOutput()
  {
    this->Update();
    return static_cast< TFixedImage* >( itk::ProcessObject::GetPrimaryOutput() );
  }

protected:

  void GenerateData( void ) ITK_OVERRIDE;

private:

  ElastixFilter();

  void AddInputAutoIncrementName( DataObjectIdentifierType key, itk::DataObject* input );

  bool IsFixedImage( DataObjectIdentifierType key );
  bool IsMovingImage( DataObjectIdentifierType key );

  void RemoveFixedImages( void );
  void RemoveMovingImages( void );

  // TODO: When set to true, ReleaseDataFlag should also touch these containers
  DataObjectContainerPointer m_FixedImageContainer;
  DataObjectContainerPointer m_MovingImageContainer;
  DataObjectContainerPointer m_FixedMaskContainer;
  DataObjectContainerPointer m_MovingMaskContainer;

  std::string m_FixedMeshFileName;
  std::string m_MovingMeshFileName;

  std::string m_OutputDirectory;
  std::string m_LogFileName;

  bool m_LogToConsole;
  bool m_LogToFile;


};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxElastixFilter.hxx"
#endif

#endif // ElastixFilter_h