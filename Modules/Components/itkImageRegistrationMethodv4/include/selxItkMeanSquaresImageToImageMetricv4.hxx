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

#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
  template< int Dimensionality, class TPixel, class InternalComputationValueType >
  ItkMeanSquaresImageToImageMetricv4Component< Dimensionality, TPixel, InternalComputationValueType >::ItkMeanSquaresImageToImageMetricv4Component()
{
  m_theItkFilter = TheItkFilterType::New();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
ItkMeanSquaresImageToImageMetricv4Component< Dimensionality, TPixel, InternalComputationValueType >::~ItkMeanSquaresImageToImageMetricv4Component()
{
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
typename ItkMeanSquaresImageToImageMetricv4Component< Dimensionality, TPixel, InternalComputationValueType >::ItkMetricv4Pointer
ItkMeanSquaresImageToImageMetricv4Component< Dimensionality, TPixel, InternalComputationValueType >::GetItkMetricv4()
{
  return (ItkMetricv4Pointer)this->m_theItkFilter;
}


template< int Dimensionality, class TPixel, class InternalComputationValueType >
bool
ItkMeanSquaresImageToImageMetricv4Component< Dimensionality, TPixel, InternalComputationValueType >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );
  bool meetsCriteria( false );
  auto status = CheckTemplateProperties(this->TemplateProperties(), criterion);
  if (status == CriterionStatus::Satisfied)
  {
    return true;
  }
  else if (status == CriterionStatus::Failed)
  {
    return false;
  } // else: CriterionStatus::Unknown
  return meetsCriteria;
}
} //end namespace selx
