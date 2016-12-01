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

#ifndef selxItkMeanSquaresImageToImageMetricv4Component_h
#define selxItkMeanSquaresImageToImageMetricv4Component_h

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkMeanSquaresImageToImageMetricv4.h"

namespace selx
{
template< int Dimensionality, class TPixel, class InternalComputationValueType >
class ItkMeanSquaresImageToImageMetricv4Component :
  public SuperElastixComponent<
  Accepting< >,
  Providing< itkMetricv4Interface< Dimensionality, TPixel, InternalComputationValueType >>
  >
{
public:

  selxNewMacro( ItkMeanSquaresImageToImageMetricv4Component, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkMeanSquaresImageToImageMetricv4Component();
  virtual ~ItkMeanSquaresImageToImageMetricv4Component();

  typedef TPixel PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef itk::Image< PixelType, Dimensionality > FixedImageType;
  typedef itk::Image< PixelType, Dimensionality > MovingImageType;
  using VirtualImageType = FixedImageType;

  typedef typename itk::ImageToImageMetricv4< FixedImageType, MovingImageType > ImageToImageMetricv4Type;
  typedef typename ImageToImageMetricv4Type::Pointer                            ItkMetricv4Pointer;

  typedef typename itk::MeanSquaresImageToImageMetricv4< FixedImageType, MovingImageType, VirtualImageType,
    InternalComputationValueType > TheItkFilterType;

  virtual ItkMetricv4Pointer GetItkMetricv4() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  //static const char * GetName() { return "ItkMeanSquaresImageToImageMetricv4"; } ;
  static const char * GetDescription() { return "ItkMeanSquaresImageToImageMetricv4 Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkMeanSquaresImageToImageMetricv4Component" }, { keys::PixelType, PodString< TPixel >::Get() },
             { keys::Dimensionality, std::to_string( Dimensionality ) },
             { keys::InternalComputationValueType, PodString< InternalComputationValueType >::Get() } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkMeanSquaresImageToImageMetricv4.hxx"
#endif
#endif // #define selxItkMeanSquaresImageToImageMetricv4Component_h
