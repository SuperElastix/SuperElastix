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

#include "selxSuperElastixComponent.h"

#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"

#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component :
  public SuperElastixComponent<
  Accepting< itkImageFixedMaskInterface< Dimensionality, unsigned char >,
             itkImageMovingMaskInterface< Dimensionality, unsigned char > >,
  Providing< itkMetricv4Interface< Dimensionality, TPixel, double >, UpdateInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<
    Dimensionality, TPixel
    >                                      Self;
  typedef SuperElastixComponent<
    Accepting< itkImageFixedMaskInterface< Dimensionality, unsigned char >,
               itkImageMovingMaskInterface< Dimensionality, unsigned char > >,
    Providing< itkMetricv4Interface< Dimensionality, TPixel, double >, UpdateInterface >
    >                                      Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component( const std::string & name, LoggerImpl & logger );
  virtual ~ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component();

  typedef TPixel PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit.
  typedef itk::Image< PixelType, Dimensionality > FixedImageType;
  typedef itk::Image< PixelType, Dimensionality > MovingImageType;

  typedef typename itk::ImageToImageMetricv4< FixedImageType, MovingImageType > ImageToImageMetricv4Type;

  typedef typename ImageToImageMetricv4Type::Pointer ItkMetricv4Pointer;

  typedef typename itk::ANTSNeighborhoodCorrelationImageToImageMetricv4< FixedImageType, MovingImageType > ANTSNeighborhoodCorrelationImageToImageMetricv4Type;

  // accepting Interfaces:
  int Accept(typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::Pointer ) override;

  int Accept(typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::Pointer) override;


  // providing Interfaces:
  ItkMetricv4Pointer GetItkMetricv4() override;

  // Base class methods:
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  bool ConnectionsSatisfied() override {return true;} // all of the accepting interfaces are optional

  static const char * GetDescription() { return "ItkANTSNeighborhoodCorrelationImageToImageMetricv4 Component"; }

  void BeforeUpdate() override;

private:

  typename ANTSNeighborhoodCorrelationImageToImageMetricv4Type::Pointer m_ANTSNeighborhoodCorrelationImageToImageMetricv4;
  typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::ItkImageType::Pointer m_FixedMask;
  typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::ItkImageType::Pointer m_MovingMask;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
