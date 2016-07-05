#ifndef selxSuperElastixFilter_h
#define selxSuperElastixFilter_h

#include "itkProcessObject.h"
#include "selxBlueprint.h"
#include "Overlord.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

/**
 * \class SuperElastixFilter
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{

template< typename ComponentTypeList >
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

  typedef AnyFileReader AnyFileReaderType;
  typedef AnyFileWriter AnyFileWriterType;

  //TODO make const correct
  //itkSetConstObjectMacro(Blueprint,Blueprint)
  itkSetObjectMacro(Blueprint, Blueprint);
  
  typename AnyFileReaderType::Pointer GetInputFileReader(const DataObjectIdentifierType&);
  typename AnyFileWriterType::Pointer GetOutputFileWriter(const DataObjectIdentifierType&);

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  void SetInput(const DataObjectIdentifierType&, InputDataType*);
  
  /** Non type-specific GetOutput */
  OutputDataType * GetOutput(const DataObjectIdentifierType&);

  /** GetOutput tries dynamic cast to required output type */
  template<typename ReturnType>
  ReturnType* GetOutput(const DataObjectIdentifierType&);

  void Update(void) ITK_OVERRIDE;
protected:

  SuperElastixFilter(void);

  virtual void GenerateOutputInformation(void) ITK_OVERRIDE;
  virtual void GenerateData(void) ITK_OVERRIDE;


private:
  //TODO make const correct
  //Blueprint::ConstPointer m_Blueprint;
  Blueprint::Pointer m_Blueprint;
  Overlord::Pointer m_Overlord;
};

} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixFilter.hxx"
#endif

#endif // selxSuperElastixFilter_h
