/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef selxInterfaces_h
#define selxInterfaces_h

#include <typeinfo>
#include <string>

#include "itkProcessObject.h"
#include "itkImageToImageFilter.h"
#include "itkImageToImageMetricv4.h"
#include "itkObjectToObjectOptimizerBase.h"
#include "itkTransformParametersAdaptorBase.h"
#include "itkGaussianExponentialDiffeomorphicTransformParametersAdaptor.h"
#include "itkGaussianExponentialDiffeomorphicTransform.h"

#include "itkImage.h"
#include "itkMesh.h"

#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"

#include "elxElastixFilter.h"

namespace selx
{
// Define the providing interfaces abstractly
class MetricDerivativeInterface
{
public:

  virtual int GetDerivative() = 0;
};

class MetricValueInterface
{
public:

  virtual int GetValue() = 0;
};

class OptimizerUpdateInterface
{
public:

  virtual int Update() = 0;
};

class TransformedImageInterface
{
public:

  virtual int GetTransformedImage() = 0;
};

class ConflictinUpdateInterface
{
public:

  // "error" : member function templates cannot be virtual
  // template <class ConflictinUpdateInterface> virtual int Update() = 0;
  //TODO http://en.cppreference.com/w/cpp/language/member_template

  //TODO solution: http://stackoverflow.com/questions/2004820/inherit-interfaces-which-share-a-method-name
  //TODO better?: http://stackoverflow.com/questions/18398409/c-inherit-from-multiple-base-classes-with-the-same-virtual-function-name
  virtual int Update( ConflictinUpdateInterface * ) = 0;
};

template< int Dimensionality, class TPixel >
class itkImageInterface
{
  // An interface that passes the pointer of an output image

public:

  typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
  virtual typename ItkImageType::Pointer GetItkImage() = 0;
};

template< int Dimensionality, class TPixel >
class itkImageFixedInterface
{
  // An interface that passes the pointer of an output image

public:

  typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
  virtual typename ItkImageType::Pointer GetItkImageFixed() = 0;
};

template< int Dimensionality >
class itkImageDomainFixedInterface
{
  // An interface that passes the pointer of an output image

public:

  typedef typename itk::ImageBase< Dimensionality > ItkImageDomainType;
  virtual typename ItkImageDomainType::Pointer GetItkImageDomainFixed() = 0;
};

template< int Dimensionality, class TPixel >
class itkImageMovingInterface
{
  // An interface that passes the pointer of an output image

public:

  typedef typename itk::Image< TPixel, Dimensionality > ItkImageType;
  virtual typename ItkImageType::Pointer GetItkImageMoving() = 0;
};

template< int Dimensionality, class TPixel >
class DisplacementFieldItkImageSourceInterface
{
  // An interface that passes the pointer of an output image

public:

  typedef typename itk::Image< itk::Vector< TPixel, Dimensionality >, Dimensionality > ItkImageType;
  virtual typename ItkImageType::Pointer GetDisplacementFieldItkImage() = 0;
};

template< int Dimensionality, class TPixel >
class itkMeshInterface
{
  // An interface that passes the pointer of an output mesh

public:

  virtual typename itk::Mesh< TPixel, Dimensionality >::Pointer GetItkMesh() = 0;
};

class SourceInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // By this interface only Source Components can to talk to the Overlord.
  // How specific Source Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  virtual void SetMiniPipelineInput( itk::DataObject::Pointer ) = 0;
  virtual AnyFileReader::Pointer GetInputFileReader( void ) = 0;
};

class SinkInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // By this interface only Sink Components can to talk to the Overlord
  // How specific Sink Components connect to the graph is up to them, i.e. they might adapt the passed Object to other types.

public:

  typedef itk::DataObject::Pointer DataObjectPointer;
  virtual void SetMiniPipelineOutput( DataObjectPointer ) = 0;
  virtual DataObjectPointer GetMiniPipelineOutput( void ) = 0;

  virtual AnyFileWriter::Pointer GetOutputFileWriter( void ) = 0;

  virtual DataObjectPointer GetInitializedOutput( void ) = 0;
};

class RegistrationControllerStartInterface
{
  // A special interface: the Overlord checks components for this type of interface.
  // This interface is to control the execution of the network

public:

  virtual void RegistrationControllerStart() = 0;
};

class RunRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  virtual void RunRegistration() = 0;
};

class AfterRegistrationInterface
{
  // This interface is to control the execution of the network

public:

  virtual void AfterRegistration() = 0;
};

class RunResolutionInterface
{
  // This interface is to control the execution of the network

public:

  virtual bool RunResolution() = 0;
};

class ReconnectTransformInterface
{
  // This interface is to control the execution of the network

public:

  virtual void ReconnectTransform() = 0;
};

template< int Dimensionality, class TPixel >
class itkMetricv4Interface
{
public:

  typedef typename itk::Image< TPixel, Dimensionality >                         FixedImageType;
  typedef typename itk::Image< TPixel, Dimensionality >                         MovingImageType;
  typedef typename itk::ImageToImageMetricv4< FixedImageType, MovingImageType > ImageToImageMetricv4Type;

  virtual typename ImageToImageMetricv4Type::Pointer GetItkMetricv4() = 0;
};

template< class TInternalComputationValueType >
class itkOptimizerv4Interface
{
public:

  /**  Type of the optimizer. */
  typedef TInternalComputationValueType                                            InternalComputationValueType;
  typedef itk::ObjectToObjectOptimizerBaseTemplate< InternalComputationValueType > OptimizerType;
  typedef typename OptimizerType::Pointer                                          Optimizerv4Pointer;
  virtual Optimizerv4Pointer GetItkOptimizerv4() = 0;
};

template< class TInternalComputationValueType, int Dimensionality >
class itkTransformInterface
{
public:

  typedef TInternalComputationValueType                                                            InternalComputationValueType;
  typedef typename itk::Transform< TInternalComputationValueType, Dimensionality, Dimensionality > TransformType;
  typedef typename TransformType::Pointer                                                          TransformPointer;

  virtual TransformPointer GetItkTransform() = 0;
};

template<class TransformInternalComputationValueType, int Dimensionality >
class itkTransformParametersAdaptorInterface
{
public:
  // TransformBaseType acts as a container of the types: TParametersValueType, NInputDimensions, NOutputDimensions
  using TransformBaseType = itk::Transform< TransformInternalComputationValueType, Dimensionality, Dimensionality >;
  using TransformParametersAdaptorBaseType = itk::TransformParametersAdaptorBase<TransformBaseType>;
  using TransformParametersAdaptorsContainerType = std::vector<typename TransformParametersAdaptorBaseType::Pointer>;

  virtual TransformParametersAdaptorsContainerType GetItkTransformParametersAdaptorsContainer() = 0;
};

template< class TransformInternalComputationValueType, int Dimensionality >
class itkGaussianExponentialDiffeomorphicTransformParametersAdaptorInterface
{
public:
  
  
  using TransformBaseType = itk::Transform< TransformInternalComputationValueType, Dimensionality, Dimensionality >;
  using TransformParametersAdaptorBaseType = itk::TransformParametersAdaptorBase<TransformBaseType>;

  using GaussianExponentialDiffeomorphicTransformType = itk::GaussianExponentialDiffeomorphicTransform< TransformInternalComputationValueType, Dimensionality >;

  using TransformParametersAdaptorType = itk::GaussianExponentialDiffeomorphicTransformParametersAdaptor< GaussianExponentialDiffeomorphicTransformType >;
  using TransformParametersAdaptorPointer = typename TransformParametersAdaptorType::Pointer;
  //using TransformParametersAdaptorsContainerType = std::vector<TransformParametersAdaptorPointer>;
  using TransformParametersAdaptorsContainerType = std::vector<typename TransformParametersAdaptorBaseType::Pointer>;
  
  //using TransformParametersAdaptorsContainerType = typename TransformParametersAdaptorType::TransformParametersAdaptorsContainerType;
  

  virtual TransformParametersAdaptorsContainerType GetItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainer() = 0;
}; 

template< typename TFixedImage, typename TMovingImage >
class elastixTransformParameterObjectInterface
{
public:

  typedef typename elastix::ElastixFilter< TFixedImage, TMovingImage >::ParameterObjectType elastixTransformParameterObject;
  virtual elastixTransformParameterObject * GetTransformParameterObject() = 0;
};
} // end namespace selx

#endif // #define selxInterfaces_h
