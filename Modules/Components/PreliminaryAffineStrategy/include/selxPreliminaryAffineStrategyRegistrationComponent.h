/*=========================================================================
 *
 *  This Preliminary Affine Strategy Module developed by Rupert Brooks in the context
 *  of the Continuous Registration Challenge.  June 2018
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

#ifndef selxPreliminaryAffineStrategyRegistrationComponent_h
#define selxPreliminaryAffineStrategyRegistrationComponent_h

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkImageSource.h"
#include <array>
#include <algorithm>
#include <string>


namespace selx
{
template< int Dimensionality, class TPixel >
class PreliminaryAffineStrategyRegistrationComponent :
  public SuperElastixComponent<
  Accepting<
  itkImageFixedInterface< Dimensionality, TPixel >,
  itkImageMovingInterface< Dimensionality, TPixel >
  >,
  Providing< UpdateInterface,
  itkDisplacementFieldInterface< Dimensionality, TPixel >
  >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef PreliminaryAffineStrategyRegistrationComponent<
    Dimensionality, TPixel
    >                                      Self;
  typedef SuperElastixComponent<
    Accepting<
    itkImageFixedInterface< Dimensionality, TPixel >,
    itkImageMovingInterface< Dimensionality, TPixel >
    >,
    Providing< UpdateInterface,
    itkDisplacementFieldInterface< Dimensionality, float >
    >
    > Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  PreliminaryAffineStrategyRegistrationComponent( const std::string & name, LoggerImpl & logger );
  virtual ~PreliminaryAffineStrategyRegistrationComponent();

  typedef typename ComponentBase::CriterionType CriterionType;
  typedef TPixel                                PixelType;

  // fixed and moving image types are all the same, these aliases can be used to be explicit. 
  using MovingImageType = typename itkImageMovingInterface< Dimensionality, TPixel >::ItkImageType;
  using ResultImageType = typename itkImageInterface< Dimensionality, TPixel >::ItkImageType;
  using DisplacementFieldType = typename itkDisplacementFieldInterface< Dimensionality, float >::ItkDisplacementFieldType;
  
  // Accepting Interfaces:
  virtual int Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer ) override;

  virtual int Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer ) override;

  // Providing Interfaces:
  virtual typename DisplacementFieldType::Pointer GetItkDisplacementField() override;

  virtual void Update() override;

  virtual bool MeetsCriterion( const CriterionType & criterion ) override;

  static const char * GetDescription() { return "PreliminaryAffineStrategyRegistration Component"; }

private:

  typename DisplacementFieldType::Pointer m_DisplacementField;
  typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer m_ImageFixedInterface;
  typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer m_ImageMovingInterface;

  std::array<float, 3> m_demoParameter;
  int m_userMode;
  std::string m_strategy;


protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "PreliminaryAffineStrategyRegistrationComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxPreliminaryAffineStrategyRegistrationComponent.hxx"
#endif
#endif // #define selxPreliminaryAffineStrategyRegistrationComponent_h
