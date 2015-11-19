#ifndef ElastixComponent_h
#define ElastixComponent_h

#include "elxMacro.h"

#include "itkProcessObject.h"
#include "itkImageSource.h"
#include "itkImage.h"
#include "itkVectorContainer.h"

#include "boost/any.hpp"

namespace selx {

template< typename TOutputImage >
class ElastixComponent : public itk::ImageSource< TOutputImage >
{
public:

  elxNewMacro( ElastixComponent, itk::ImageSource );

  void SetFixedImage( void );

  typedef unsigned int                                    InputIdType;
  typedef itk::VectorContainer< InputIdType, boost::any > InputContainerType;

private:

  InputContainerType m_InputContainer;

};

} // namespace selx

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxElastixComponent.hxx"
#endif

#endif // ElastixComponent_h