#ifndef selxFileWriterDecorator_h
#define selxFileWriterDecorator_h

#include "selxAnyFileWriter.h"

/**
 * \class selxFileWriterDecorator
 * \brief Wrapper class, for a template specifiable Writer, that can be casted to an AnyFileWriter base class.
 */

namespace selx
{

template< typename TWriter>
class FileWriterDecorator : public AnyFileWriter
{
public:

  /** Standard ITK typedefs. */
  typedef FileWriterDecorator                 Self;
  typedef AnyFileWriter Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  typedef TWriter WriterType;
  typedef typename TWriter::Pointer      WriterPointer;
  typedef typename TWriter::ConstPointer WriterConstPointer;
  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro(Self, selxAnyFileWriter);
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  //TODO: DerivedInputDataType is defined as an ImageType, which is should be an MeshType if WriterType is a MeshReader
  //Use a Traits Class for this with template specialization to Images, Meshes etc.
  typedef typename WriterType::InputImageType DerivedInputDataType;
  
  virtual void SetFileName(const std::string) ITK_OVERRIDE;

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  virtual void SetInput(const InputDataType *) ITK_OVERRIDE;

  virtual void Update(void) ITK_OVERRIDE;

  FileWriterDecorator(void);
  ~FileWriterDecorator(void);
protected:

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;
  
private:
  WriterPointer m_Writer;
};

} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxFileWriterDecorator.hxx"
#endif

#endif // selxProcessObject_h
