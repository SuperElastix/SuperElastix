
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

#ifndef selxDisplacementFieldImageWarperComponent_hxx
#define selxDisplacementFieldImageWarperComponent_hxx

#include "selxDisplacementFieldImageWarperComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx {

template< int Dimensionality, class TPixel, class CoordRepType >
ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >
::ItkDisplacementFieldImageWarperComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger ) {
  this->m_DisplacementFieldTransform = DisplacementFieldTransformType::New();
  this->m_ResampleImageFilter = ResampleImageFilterType::New();
  this->m_ResampleImageFilter->SetTransform(this->m_DisplacementFieldTransform.GetPointer());
  this->m_UseNearestNeighborInterpolation = false;
};

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >
::Accept( DisplacementFieldInterfacePointer itkDisplacementFieldInterface )
{
  auto displacementField = itkDisplacementFieldInterface->GetItkDisplacementField();
  displacementField->Update();
  this->m_DisplacementFieldTransform->SetDisplacementField( displacementField );
  this->m_ResampleImageFilter->SetOutputParametersFromImage( displacementField );

  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
int
ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >
::Accept( MovingImageInterfacePointer movingImageInterface )
{
  this->m_ResampleImageFilter->SetInput( movingImageInterface->GetItkImageMoving() );

  return 0;
}

template< int Dimensionality, class TPixel, class CoordRepType >
typename ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >::ResultImagePointer
ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >
::GetItkImage()
{
  return this->m_ResampleImageFilter->GetOutput();
}

template< int Dimensionality, class TPixel, class CoordRepType >
bool
ItkDisplacementFieldImageWarperComponent< Dimensionality, TPixel, CoordRepType >
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
  }
  else if( criterion.first == "Interpolator" && criterion.second.size() > 0 )
  {
    if( criterion.second[0] == "NearestNeighbor" ) {
      NearestNeighborInterpolatorTypePointer nearestNeighborInterpolator = NearestNeighborInterpolatorType::New();
      this->m_ResampleImageFilter->SetInterpolator(nearestNeighborInterpolator);
      return true;
    }
  }

  return meetsCriteria;
}

} // namespace selx

#endif // selxDisplacementFieldImageWarperComponent_hxx