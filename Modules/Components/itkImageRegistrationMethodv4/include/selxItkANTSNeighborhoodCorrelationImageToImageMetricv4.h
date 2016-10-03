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

#ifndef selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component_h
#define selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component_h

#include "selxComponentBase.h"
#include "selxInterfaces.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include <string.h>
#include "selxMacro.h"
namespace selx
{
template< int Dimensionality, class TPixel >
class ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component :
  public SuperElastixComponent<
  Accepting< >,
  Providing< itkMetricv4Interface< Dimensionality, TPixel, double >>
  >
{
public:

  selxNewMacro( ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component, ComponentBase );

  //itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component();
  virtual ~ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component();

  typedef TPixel PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef itk::Image< PixelType, Dimensionality > FixedImageType;
  typedef itk::Image< PixelType, Dimensionality > MovingImageType;

  typedef typename itk::ImageToImageMetricv4< FixedImageType, MovingImageType > ImageToImageMetricv4Type;
  //typedef typename ItkMetricv4Interface<Dimensionality, TPixel>::ImageToImageMetricv4Type ImageToImageMetricv4Type;
  //typedef ItkMetricv4Interface<Dimensionality, TPixel>::ImageToImageMetricv4Type ItkMetricv4Pointer
  typedef typename ImageToImageMetricv4Type::Pointer ItkMetricv4Pointer;

  typedef typename itk::ANTSNeighborhoodCorrelationImageToImageMetricv4< FixedImageType, MovingImageType > TheItkFilterType;

  virtual ItkMetricv4Pointer GetItkMetricv4() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkANTSNeighborhoodCorrelationImageToImageMetricv4 Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
