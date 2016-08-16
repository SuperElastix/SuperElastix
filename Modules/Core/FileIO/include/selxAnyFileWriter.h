#ifndef AnyFileWriter_h
#define AnyFileWriter_h

#include "itkProcessObject.h"

/**
* \class selxFileWriterDecorator
* \brief A base class to handle any file Writer that derives from this class.
*/

namespace selx
{
class AnyFileWriter : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef AnyFileWriter                   Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::ProcessObject );
  typedef itk::DataObject InputDataType;

  virtual void SetFileName( const std::string ) = 0;

  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  virtual void SetInput( const InputDataType * ) = 0;

  virtual void Update( void ) = 0;

protected:

  //AnyFileWriter(void) {};
  //~AnyFileWriter(void) {};

  //virtual void GenerateData(void) ITK_OVERRIDE;
  //virtual void GenerateOutputInformation(void) ITK_OVERRIDE;
};
} // namespace elx

#endif // AnyFileWriter_h
