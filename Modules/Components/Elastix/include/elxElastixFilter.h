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
 */

namespace selx {

// PixelType traits for writing types as strings to parameter files
template < typename T >
struct TypeName
{
  static const char* ToString()
  {
     itkGenericExceptionMacro( "Pixel type \"" << typeid(T).name() << "\" not supported by elastix." )
  }
};

template <>
struct TypeName< char >
{
  static const char* ToString()
  {
    return "char";
  }
};

template <>
struct TypeName< unsigned char >
{
  static const char* ToString()
  {
    return "unsigned char";
  }
};

template <>
struct TypeName< short >
{
  static const char* ToString()
  {
    return "short";
  }
};

template <>
struct TypeName< unsigned short >
{
  static const char* ToString()
  {
    return "unsigned short";
  }
};

template <>
struct TypeName< int >
{
  static const char* ToString()
  {
    return "int";
  }
};

template <>
struct TypeName< unsigned int >
{
  static const char* ToString()
  {
    return "unsigned int";
  }
};

template <>
struct TypeName< long >
{
  static const char* ToString()
  {
    return "long";
  }
};

template <>
struct TypeName< unsigned long >
{
  static const char* ToString()
  {
    return "unsigned long";
  }
};

template <>
struct TypeName< float >
{
  static const char* ToString()
  {
    return "float";
  }
};

template <>
struct TypeName< double >
{
  static const char* ToString()
  {
    return "double";
  }
};

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

  typedef ParameterObject::ParameterMapVectorType             ParameterMapVectorType;
  typedef ParameterObject::ParameterMapType                   ParameterMapType;
  typedef ParameterObject::ParameterValueVectorType           ParameterValueVectorType;
  typedef ParameterObject::Pointer                            ParameterObjectPointer;
  typedef ParameterObject::ConstPointer                       ParameterObjectConstPointer;

  typedef typename TFixedImage::Pointer                       FixedImagePointer;
  typedef typename TMovingImage::Pointer                      MovingImagePointer;

  itkStaticConstMacro( FixedImageDimension, unsigned int, TFixedImage::ImageDimension );
  itkStaticConstMacro( MovingImageDimension, unsigned int, TMovingImage::ImageDimension );

  void SetFixedImage( FixedImagePointer fixedImage );
  void SetFixedImage( DataObjectContainerPointer fixedImages );
  void AddFixedImage( FixedImagePointer fixedImage );

  void SetMovingImage( MovingImagePointer movingImages );
  void SetMovingImage( DataObjectContainerPointer movingImages );
  void AddMovingImage( MovingImagePointer movingImage );

  void SetFixedMask( FixedImagePointer fixedMask );
  void SetFixedMask( DataObjectContainerPointer fixedMasks );
  void AddFixedMask( FixedImagePointer fixedMask );
  void RemoveFixedMask( void );

  void SetMovingMask( MovingImagePointer movingMask );
  void SetMovingMask( DataObjectContainerPointer movingMasks );
  void AddMovingMask( MovingImagePointer movingMask );
  void RemoveMovingMask( void );

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

  FixedImagePointer GetOutput( void );

protected:

  void GenerateData( void ) ITK_OVERRIDE;

private:

  ElastixFilter();

  void AddInputAutoIncrementName( DataObjectIdentifierType key, itk::DataObject* input );

  bool IsInputType( DataObjectIdentifierType inputType, DataObjectIdentifierType inputName );
  void RemoveInputType( DataObjectIdentifierType inputName );

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