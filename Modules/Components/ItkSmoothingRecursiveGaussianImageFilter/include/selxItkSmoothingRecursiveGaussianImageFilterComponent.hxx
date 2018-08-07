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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >::ItkSmoothingRecursiveGaussianImageFilterComponent(
  const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  m_theItkFilter = TheItkFilterType::New();
}


template< int Dimensionality, class TPixel >
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >::~ItkSmoothingRecursiveGaussianImageFilterComponent()
{
}


template< int Dimensionality, class TPixel >
int
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >
::Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer component )
{
  auto image = component->GetItkImage();
  // connect the itk pipeline
  this->m_theItkFilter->SetInput( image );
  return 0;
}


//ItkImageSourceType::Pointer
template< int Dimensionality, class TPixel >
typename ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >::ItkImagePointer
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >
::GetItkImage()
{
  return m_theItkFilter->GetOutput();
}


template< int Dimensionality, class TPixel >
bool
ItkSmoothingRecursiveGaussianImageFilterComponent< Dimensionality, TPixel >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  } // else: CriterionStatus::Unknown

  else if( criterion.first == "Sigma" ) //Supports this?
  {
    if( criterion.second.size() != 1 )
    {
      meetsCriteria = false;
      //itkExceptionMacro("The criterion Sigma may have only 1 value");
    }
    else
    {
      auto const & criterionValue = *criterion.second.begin();
      try
      {
        this->m_theItkFilter->SetSigma( std::stod( criterionValue ) );
        meetsCriteria = true;
      }
      catch( itk::ExceptionObject & itkNotUsed(err) )
      {
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  return meetsCriteria;
}
} //end namespace selx
