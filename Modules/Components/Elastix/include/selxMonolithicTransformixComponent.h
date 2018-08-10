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
  elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality >>
  >,
  Providing<
  itkImageInterface< Dimensionality, TPixel >,
  itkDisplacementFieldInterface< Dimensionality, TPixel >,
  UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef MonolithicTransformixComponent<
    Dimensionality, TPixel
    >                                      Self;
  typedef SuperElastixComponent<
    Accepting<
    itkImageDomainFixedInterface< Dimensionality >,
    itkImageMovingInterface< Dimensionality, TPixel >,
    elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality >>
    >,
    Providing<
    itkImageInterface< Dimensionality, TPixel >,
    itkDisplacementFieldInterface< Dimensionality, TPixel >,
    UpdateInterface
    >
    >                                      Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  MonolithicTransformixComponent( const std::string & name, LoggerImpl & logger );
  virtual ~MonolithicTransformixComponent();

  typedef typename ComponentBase::CriterionType CriterionType;
  typedef TPixel                                PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit. 
  using MovingImageType = typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType;
  using ResultImageType = typename itkImageInterface< Dimensionality, TPixel >::ItkImageType;
  using ItkDisplacementFieldType = typename itkDisplacementFieldInterface< Dimensionality, TPixel >::ItkDisplacementFieldType;

  typedef elastix::ParameterObject        elxParameterObjectType;
  typedef elxParameterObjectType::Pointer elxParameterObjectPointer;

  typedef elastix::TransformixFilter< MovingImageType > TransformixFilterType;

  typedef elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >,
    itk::Image< TPixel, Dimensionality >> elastixTransformParameterObjectInterfaceType;

  // Accepting Interfaces:
  virtual int Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer ) override;

  virtual int Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual int Accept( typename elastixTransformParameterObjectInterfaceType::Pointer ) override;

  // Providing Interfaces:
  virtual typename ResultImageType::Pointer GetItkImage() override;

  virtual typename ItkDisplacementFieldType::Pointer GetItkDisplacementField() override;

  virtual void Update() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  static const char * GetDescription() { return "MonolithicTransformix Component"; }

private:

  typename TransformixFilterType::Pointer m_transformixFilter;
  typename elastixTransformParameterObjectInterfaceType::Pointer m_TransformParameterObjectInterface;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "MonolithicTransformixComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxMonolithicTransformixComponent.hxx"
#endif
#endif // #define selxMonolithicTransformixComponent_h
