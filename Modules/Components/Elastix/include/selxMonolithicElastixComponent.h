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
#include "selxElastixInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkImageSource.h"
#include "elxElastixFilter.h"
#include "elxParameterObject.h"
#include "elxTransformixFilter.h"

#include <string.h>
namespace selx
{
template< int Dimensionality, class TPixel >
class MonolithicElastixComponent :
  public SuperElastixComponent<
  Accepting<
  itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >,
  itkImageFixedMaskInterface< Dimensionality, unsigned char >,
  itkImageMovingMaskInterface< Dimensionality, unsigned char >
  >,
  Providing<
  elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality >>,
  itkImageInterface< Dimensionality, TPixel >,
  UpdateInterface
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef MonolithicElastixComponent<Dimensionality, TPixel> Self;
  typedef SuperElastixComponent<
    Accepting<
    itkImageFixedInterface< Dimensionality, TPixel >,
    itkImageMovingInterface< Dimensionality, TPixel >,
    itkImageFixedMaskInterface< Dimensionality, unsigned char >,
    itkImageMovingMaskInterface< Dimensionality, unsigned char >
    >,
    Providing<
    elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >, itk::Image< TPixel, Dimensionality >>,
    itkImageInterface< Dimensionality, TPixel >,
    UpdateInterface
    >
    >                                     Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  MonolithicElastixComponent( const std::string & name, LoggerImpl & logger );
  virtual ~MonolithicElastixComponent();

  typedef typename ComponentBase::CriterionType CriterionType;
  typedef TPixel PixelType;

  // the in and output image type of the component are chosen to be the same
  typedef itk::Image< PixelType, Dimensionality > ConnectionImageType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef itk::Image< PixelType, Dimensionality > FixedImageType;
  typedef itk::Image< PixelType, Dimensionality > MovingImageType;

  typedef typename ConnectionImageType::Pointer ItkImagePointer;

  typedef elastix::ElastixFilter< FixedImageType, MovingImageType > ElastixFilterType;
  typedef typename ElastixFilterType::Pointer ElastixFilterPointer;
  typedef elastix::ParameterObject ParameterObjectType;
  typedef typename ParameterObjectType::Pointer ParameterObjectPointer;

  typedef typename elastixTransformParameterObjectInterface< itk::Image< TPixel, Dimensionality >,
    itk::Image< TPixel, Dimensionality >>::elastixTransformParameterObject elastixTransformParameterObject;

  // Accepting Interfaces:
  virtual int Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual int Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual int Accept( typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::Pointer ) override;

  virtual int Accept( typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::Pointer ) override;

  // Providing Interfaces:
  virtual elastixTransformParameterObject * GetTransformParameterObject() override;

  virtual ItkImagePointer GetItkImage() override;

  virtual void Update() override;

  //Base class methods:
  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  virtual bool ConnectionsSatisfied() override;

  static const char * GetDescription() { return "MonolithicElastix Component"; }

private:

  ElastixFilterPointer m_ElastixFilter;
  ParameterObjectPointer m_ParameterObject;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "MonolithicElastixComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxMonolithicElastixComponent.hxx"
#endif
#endif // #define selxMonolithicElastix_h
