#ifndef ElastixComponent_h
#define ElastixComponent_h

// ITK
#include "itkProcessObject.h"
#include "itkImageSource.h"

// Elastix
#include "elxElastixMain.h"

// SuperElastix
#include "elxMacro.h"
#include "elxParameterObject.h"

namespace selx {

template< typename TFixedImage,
          typename TMovingImage,
          typename TOutputImage >
class ElastixFilter : public itk::ImageSource< TOutputImage >
{
public:

  elxNewMacro( ElastixFilter, itk::ImageSource );

  typedef ParameterObject::ParameterMapType ParameterMapType;

  typedef elastix::ElastixMain                          ElastixMainType;
  typedef ElastixMainType::Pointer                      ElastixMainPointer;
  typedef std::vector< ElastixMainPointer >             ElastixMainVectorType;
  typedef ElastixMainType::ObjectPointer                ElastixMainObjectPointer;

  typedef ElastixMainType::DataObjectContainerType      DataObjectContainerType;
  typedef ElastixMainType::DataObjectContainerPointer   DataObjectContainerPointer;

  typedef ElastixMainType::ArgumentMapType::value_type  ArgumentMapEntryType;
  typedef ElastixMainType::ArgumentMapType              ArgumentMapType;

  typedef ParameterObject::ParameterMapListType         ParameterMapListType;
  typedef ParameterObject::ParameterValuesType          ParameterValuesType;

  typedef ElastixMainType::FlatDirectionCosinesType     FlatDirectionCosinesType;

  typedef typename TFixedImage::Pointer FixedImagePointer;
  typedef typename TMovingImage::Pointer MovingImagePointer;
  typedef typename ParameterObject::Pointer ParameterObjectPointer;

  void SetFixedImage( FixedImagePointer fixedImage );
  void SetMovingImage( MovingImagePointer movingImage );
  void SetParameterObject( ParameterObjectPointer parameterObject );

  itkSetMacro( LogToConsole, bool );
  itkGetMacro( LogToConsole, bool );
  itkBooleanMacro( LogToConsole );

  itkSetMacro( LogToFile, std::string );
  itkGetMacro( LogToFile, std::string );

protected:

  void GenerateData( void ) ITK_OVERRIDE;

private:

  ElastixFilter();

  bool m_LogToConsole;
  std::string m_LogToFile;

};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxElastixFilter.hxx"
#endif

#endif // ElastixFilter_h