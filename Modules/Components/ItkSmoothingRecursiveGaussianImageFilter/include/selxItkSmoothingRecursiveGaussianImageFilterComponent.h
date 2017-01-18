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

#ifndef selxItkSmoothingRecursiveGaussianImageFilterComponent_h
#define selxItkSmoothingRecursiveGaussianImageFilterComponent_h

#include "selxSuperElastixComponent.h"

//#include "selxItkRegistrationMethodv4Interfaces.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"

#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkImageSource.h"

namespace selx
{
template< int Dimensionality, class TPixel >
class ItkSmoothingRecursiveGaussianImageFilterComponent :
  public SuperElastixComponent<
  Accepting< itkImageInterface< Dimensionality, TPixel > >,
  Providing< itkImageInterface< Dimensionality, TPixel > >
  >
{
public:

  selxNewMacro( ItkSmoothingRecursiveGaussianImageFilterComponent, ComponentBase );

  // itkStaticConstMacro(Dimensionality, unsigned int, Dimensionality);

  ItkSmoothingRecursiveGaussianImageFilterComponent();
  virtual ~ItkSmoothingRecursiveGaussianImageFilterComponent();

  typedef TPixel PixelType;
  typedef itk::SmoothingRecursiveGaussianImageFilter< itk::Image< PixelType, Dimensionality >,
    itk::Image< PixelType, Dimensionality > > TheItkFilterType;
  typedef itk::Image< PixelType, Dimensionality > ItkImageType;
  typedef typename ItkImageType::Pointer
    ItkImagePointer;

  virtual int Set( itkImageInterface< Dimensionality, TPixel > * ) override;

  virtual ItkImagePointer GetItkImage() override;

  //virtual bool MeetsCriteria(const CriteriaType &criteria);
  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "ItkSmoothingRecursiveGaussianImageFilter Component"; }

private:

  typename TheItkFilterType::Pointer m_theItkFilter;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return{ { keys::NameOfClass, "ItkSmoothingRecursiveGaussianImageFilterComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, std::to_string(Dimensionality) } };
  }
};

} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.hxx"
#endif
#endif // #define GDOptimizer3rdPartyComponent_h
