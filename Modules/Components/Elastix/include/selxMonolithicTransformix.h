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

#ifndef selxMonolithicTransformixComponent_h
#define selxMonolithicTransformixComponent_h

#include "selxSuperElastixComponent.h"
#include "selxElastixInterfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkImageSource.h"
#include "elxElastixFilter.h"
#include "elxParameterObject.h"
#include "elxTransformixFilter.h"

#include <string.h>

namespace selx
{
template< int Dimensionality, class TPixel >
class MonolithicTransformixComponent :
  public SuperElastixComponent<
  Accepting<
  itkImageDomainFixedInterface< Dimensionality >,
  itkImageMovingInterface< Dimensionality, TPixel >,
  elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality > >
  >,
  Providing<
  itkImageInterface< Dimensionality, TPixel >,
  ReconnectTransformInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef MonolithicTransformixComponent          Self;
  typedef ComponentBase                       Superclass;
  typedef itk::SmartPointer< Self >           Pointer;
  typedef itk::SmartPointer< const Self >     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  MonolithicTransformixComponent();
  virtual ~MonolithicTransformixComponent();

  typedef typename ComponentBase::CriterionType CriterionType;
  typedef TPixel                                PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType MovingImageType;
  typedef typename itkImageInterface< Dimensionality, TPixel >::ItkImageType       ResultImageType;

  typedef elastix::ParameterObject        elxParameterObjectType;
  typedef elxParameterObjectType::Pointer elxParameterObjectPointer;

  typedef elastix::TransformixFilter< MovingImageType > TransformixFilterType;

  typedef elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >,
    itk::Image< TPixel, Dimensionality > > elastixTransformParameterObjectInterfaceType;
  // Accepting Interfaces:
  virtual int Set( itkImageDomainFixedInterface< Dimensionality > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( elastixTransformParameterObjectInterfaceType * ) override;

  // Providing Interfaces:
  virtual typename ResultImageType::Pointer GetItkImage() override;

  virtual void ReconnectTransform() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  static const char * GetDescription() { return "MonolithicTransformix Component"; }

private:

  typename TransformixFilterType::Pointer m_transformixFilter;
  elastixTransformParameterObjectInterfaceType * m_TransformParameterObjectInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "MonolithicTransformixComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxMonolithicTransformix.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
