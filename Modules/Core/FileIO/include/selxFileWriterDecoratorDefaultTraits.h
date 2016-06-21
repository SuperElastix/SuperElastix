#include "itkImageFileWriter.h"
#include "itkMeshFileWriter.h"

template <typename T>
struct FileWriterDecoratorDefaultTraits
{
  struct PLEASE_IMPLEMENT_FILEWRITERDECORATOR_FOR_THIS_WRITERTYPE; 
  //TODO message produced by BOOST_MPL_ASSERT_MSG is not yet very clear
  //BOOST_MPL_ASSERT_MSG(false, PLEASE_IMPLEMENT_FILEWRITERDECORATOR_FOR_THIS_WRITERTYPE, (T));
  typedef typename bool DerivedInputDataType;
};

template<typename T1>
struct FileWriterDecoratorDefaultTraits < itk::ImageFileWriter<T1> >
{
  typedef typename itk::ImageFileWriter<T1>::InputImageType DerivedInputDataType;
};

template<typename T1>
struct FileWriterDecoratorDefaultTraits < itk::MeshFileWriter<T1> >
{
  typedef typename itk::MeshFileWriter<T1>::InputMeshType DerivedInputDataType;
};

