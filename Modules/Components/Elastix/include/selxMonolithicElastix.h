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

#ifndef selxMonolithicElastixComponent_h
#define selxMonolithicElastixComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "itkImageSource.h"

#include "elxElastixFilter.h"
#include "elxParameterObject.h"
#include "elxTransformixFilter.h"

#include <string.h>
#include "selxMacro.h"
namespace selx
{
template< int Dimensionality, class TPixel >
class MonolithicElastixComponent :
  public SuperElastixComponent<
  Accepting<
  itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >
  >,
  Providing<
  elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality > >,
  itkImageInterface< Dimensionality, TPixel >,
  RunRegistrationInterface
  >
  >
{
public:

  selxNewMacro( MonolithicElastixComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  MonolithicElastixComponent();
  virtual ~MonolithicElastixComponent();

  typedef typename ComponentBase::CriterionType CriterionType;
  typedef TPixel                                PixelType;

  // the in and output image type of the component are chosen to be the same
  typedef itk::Image< PixelType, Dimensionality > ConnectionImageType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef itk::Image< PixelType, Dimensionality > FixedImageType;
  typedef itk::Image< PixelType, Dimensionality > MovingImageType;

  typedef typename ConnectionImageType::Pointer ItkImagePointer;

  typedef elastix::ElastixFilter< FixedImageType, MovingImageType > ElastixFilterType;
  typedef elastix::ParameterObject                                  elxParameterObjectType;
  typedef elxParameterObjectType::Pointer                           elxParameterObjectPointer;

  typedef typename elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >,
    itk::Image< TPixel, Dimensionality > >::elastixTransformParameterObject elastixTransformParameterObject;

  // Accepting Interfaces:
  virtual int Set( itkImageFixedInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  // Providing Interfaces:
  virtual elastixTransformParameterObject * GetTransformParameterObject() override;

  virtual ItkImagePointer GetItkImage() override;

  virtual void RunRegistration() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  static const char * GetDescription() { return "MonolithicElastix Component"; }

private:

  typename ElastixFilterType::Pointer m_elastixFilter;

protected:

  /* The following struct returns the string name of computation type */
  /* default implementation */

  static inline const std::string GetTypeNameString()
  {
    itkGenericExceptionMacro( << "Unknown ScalarType" << typeid( TPixel ).name() );
    // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
    // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
  }


  static inline const std::string GetPixelTypeNameString()
  {
    itkGenericExceptionMacro( << "Unknown PixelType" << typeid( TPixel ).name() );
    // TODO: provide the user instructions how to enable the compilation of the component with the required template types (if desired)
    // We might define an exception object that can communicate various error messages: for simple user, for developer user, etc
  }
};

// unfortunately partial specialization of member functions is not allowed, without partially specializing the entire class.

/*
template <int Dimensionality>
class MonolithicElastixComponent < Dimensionality, double >
{
  static inline const std::string GetPixelTypeNameString();
};

template <int Dimensionality>
inline const std::string
  MonolithicElastixComponent<Dimensionality, double>
  ::GetPixelTypeNameString()
{
  return std::string("double");
}
*/

template< >
inline const std::string
MonolithicElastixComponent< 2, float >
::GetPixelTypeNameString()
{
  return std::string( "float" );
}


template< >
inline const std::string
MonolithicElastixComponent< 2, double >
::GetPixelTypeNameString()
{
  return std::string( "double" );
}


template< >
inline const std::string
MonolithicElastixComponent< 3, float >
::GetPixelTypeNameString()
{
  return std::string( "float" );
}


template< >
inline const std::string
MonolithicElastixComponent< 3, double >
::GetPixelTypeNameString()
{
  return std::string( "double" );
}


template< >
inline const std::string
MonolithicElastixComponent< 2, float >
::GetTypeNameString()
{
  return std::string( "2_float" );
}


template< >
inline const std::string
MonolithicElastixComponent< 2, double >
::GetTypeNameString()
{
  return std::string( "2_double" );
}


template< >
inline const std::string
MonolithicElastixComponent< 3, float >
::GetTypeNameString()
{
  return std::string( "3_float" );
}


template< >
inline const std::string
MonolithicElastixComponent< 3, double >
::GetTypeNameString()
{
  return std::string( "3_double" );
}
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxMonolithicElastix.hxx"
#endif
#endif // #define selxMonolithicElastix_h
