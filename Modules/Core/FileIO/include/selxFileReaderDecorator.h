#ifndef selxFileReaderDecorator_h
#define selxFileReaderDecorator_h

#include "selxAnyFileReader.h"

/**
 * \class selxFileReaderDecorator
 * \brief Wrapper class, for a template specifiable reader, that can be casted to an AnyFileReader base class.
 */

namespace selx
{
template< typename TReader >
class FileReaderDecorator : public AnyFileReader
{
public:

  /** Standard ITK typedefs. */
  typedef FileReaderDecorator             Self;
  typedef AnyFileReader                   Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  typedef TReader                        ReaderType;
  typedef typename TReader::Pointer      ReaderPointer;
  typedef typename TReader::ConstPointer ReaderConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, selxAnyFileReader );
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  virtual void SetFileName( const std::string ) ITK_OVERRIDE;

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  //void SetInput(const DataObjectIdentifierType&, InputDataType*) ITK_OVERRIDE;

  /** Non type-specific GetOutput */
  virtual OutputDataType * GetOutput() ITK_OVERRIDE;

  virtual void Update( void ) ITK_OVERRIDE;

  /** GetOutput tries dynamic cast to required output type */
  //template<typename ReturnType>
  //ReturnType* GetOutput(const DataObjectIdentifierType&);
  FileReaderDecorator();
  ~FileReaderDecorator();

protected:

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;

private:

  ReaderPointer m_Reader;
};
} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxFileReaderDecorator.hxx"
#endif

#endif // selxProcessObject_h
