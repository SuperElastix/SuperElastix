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

#ifndef selxElastixComponent_h
#define selxElastixComponent_h

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
class ElastixComponent :
  public SuperElastixComponent<
  Accepting<
  itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >
  >,
  Providing<
  itkImageInterface< Dimensionality, TPixel >,
  RunRegistrationInterface
  >
  >
{
public:

  selxNewMacro( ElastixComponent, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ElastixComponent();
  virtual ~ElastixComponent();

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

  typedef elastix::TransformixFilter< FixedImageType > TransformixFilterType;

  typedef itk::ResampleImageFilter< MovingImageType, ConnectionImageType > ResampleFilterType;

  virtual int Set( itkImageFixedInterface< Dimensionality, TPixel > * ) override;

  virtual int Set( itkImageMovingInterface< Dimensionality, TPixel > * ) override;

  virtual ItkImagePointer GetItkImage() override;

  virtual void RunRegistration() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  static const char * GetDescription() { return "Elastix Component"; }

private:

  typename ElastixFilterType::Pointer m_elastixFilter;
  typename TransformixFilterType::Pointer m_transformixFilter;
  //selxParameterObjectPointer m_ParameterObject;

  ItkImagePointer m_OutputImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ElastixComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxElastixComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
