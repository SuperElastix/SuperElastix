#ifndef selxSuperElastixFilter_h
#define selxSuperElastixFilter_h

#include "itkImageSource.h"
#include "itkImageToImageFilter.h"
#include "itkMesh.h"

/**
 * \class SuperElastixFilter
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{

template< typename TFixedImage, typename TMovingImage >
class SuperElastixFilter : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef SuperElastixFilter                   Self;
  typedef itk::ProcessObject Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro(Self, itk::ProcessObject);
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  typedef itk::Image<bool,1> DummyDataObjectType;

  void SetConfiguration();

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  void SetInput(const DataObjectIdentifierType&, InputDataType*) ITK_OVERRIDE;
  
  /** Non type-specific GetOutput */
  OutputDataType * GetOutput(const DataObjectIdentifierType&) ITK_OVERRIDE;

  /** GetOutput tries dynamic cast to required output type */
  template<typename ReturnType>
  ReturnType* GetOutput(const DataObjectIdentifierType&);

protected:

  SuperElastixFilter(void);

  virtual void GenerateData(void) ITK_OVERRIDE;
  virtual void GenerateOutputInformation(void) ITK_OVERRIDE;

};

} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixFilter.hxx"
#endif

#endif // selxSuperElastixFilter_h
