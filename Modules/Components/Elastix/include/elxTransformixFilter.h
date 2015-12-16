#ifndef TransformixFilter_h
#define TransformixFilter_h

// ITK
#include "itkImageSource.h"

// Transformix
#include "elxTransformixMain.h"

// SuperElastix
#include "elxMacro.h"
#include "elxParameterObject.h"

namespace selx {

template< typename TInputImage >
class TransformixFilter : public itk::ImageSource< TInputImage >
{
public:

  elxNewMacro( TransformixFilter, itk::ImageSource );

  typedef elastix::TransformixMain                          TransformixMainType;
  typedef TransformixMainType::Pointer                      TransformixMainPointer;
  typedef TransformixMainType::ArgumentMapType              ArgumentMapType;
  typedef ArgumentMapType::value_type                       ArgumentMapEntryType;

  typedef itk::ProcessObject::DataObjectIdentifierType      DataObjectIdentifierType;
  typedef TransformixMainType::DataObjectContainerType      DataObjectContainerType;
  typedef TransformixMainType::DataObjectContainerPointer   DataObjectContainerPointer;

  typedef ParameterObject::ParameterMapListType             ParameterMapListType;
  typedef ParameterObject::ParameterMapType                 ParameterMapType;
  typedef ParameterObject::ParameterVectorType              ParameterVectorType;
  typedef typename ParameterObject::Pointer                 ParameterObjectPointer;
  typedef typename ParameterObject::ConstPointer            ParameterObjectConstPointer;

  typedef typename TInputImage::Pointer                     InputImagePointer;

  void SetInputImage( InputImagePointer inputImage );

  void SetTransformParameters( ParameterObjectPointer parameterObject );
  ParameterObjectPointer GetTransformParameters( void );

  itkSetMacro( ComputeSpatialJacobian, bool );
  itkGetConstMacro( ComputeSpatialJacobian, bool );
  itkBooleanMacro( ComputeSpatialJacobian );

  itkSetMacro( ComputeDeterminantOfSpatialJacobian, bool );
  itkGetConstMacro( ComputeDeterminantOfSpatialJacobian, bool );
  itkBooleanMacro( ComputeDeterminantOfSpatialJacobian );

  itkSetMacro( ComputeDeformationField, bool );
  itkGetConstMacro( ComputeDeformationField, bool );
  itkBooleanMacro( ComputeDeformationField );

  itkSetMacro( PointSetFileName, std::string );
  itkGetConstMacro( PointSetFileName, std::string );
  void DeletePointSetFileName() { this->m_PointSetFileName = std::string(); };

  itkSetMacro( OutputDirectory, std::string );
  itkGetConstMacro( OutputDirectory, std::string );
  void DeleteOutputDirectory() { this->m_OutputDirectory = std::string(); };

  itkSetMacro( LogToConsole, bool );
  itkGetConstMacro( LogToConsole, bool );
  itkBooleanMacro( LogToConsole );

  itkSetMacro( LogToFile, bool );
  itkGetConstMacro( LogToFile, bool );
  itkBooleanMacro( LogToFile );

protected:

  void GenerateData( void ) ITK_OVERRIDE;

private:

  TransformixFilter();

  bool          m_ComputeSpatialJacobian;
  bool          m_ComputeDeterminantOfSpatialJacobian;
  bool          m_ComputeDeformationField;
  std::string   m_PointSetFileName;

  std::string   m_OutputDirectory;
  bool          m_LogToConsole;
  bool          m_LogToFile;
  

};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxTransformixFilter.hxx"
#endif

#endif // TransformixFilter_h