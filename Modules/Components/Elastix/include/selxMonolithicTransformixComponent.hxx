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

#include "selxMonolithicTransformixComponent.h"
#include "selxCheckTemplateProperties.h"
#include <string>

namespace selx
{
template< int Dimensionality, class TPixel >
MonolithicTransformixComponent< Dimensionality, TPixel >::MonolithicTransformixComponent( const std::string & name,
  LoggerImpl & logger ) : Superclass( name, logger )
{
  m_transformixFilter = TransformixFilterType::New();
  m_transformixFilter->LogToConsoleOn();
  m_transformixFilter->LogToFileOff();
  m_transformixFilter->SetOutputDirectory( "." );

  //TODO m_elastixFilter returns a nullptr GetTransformParameterObject instead of a valid object. However, we need this object to satisfy the input conditions of m_transformixFilter
  elxParameterObjectPointer trxParameterObject = elxParameterObjectType::New();
  //typename elxParameterObjectType::ParameterMapType defaultParameters = elxParameterObject->GetParameterMap("rigid");
  //elxParameterObject->SetParameterMap(defaultParameters);
  //m_transformixFilter->SetTransformParameterObject(m_elastixFilter->GetTransformParameterObject());
  m_transformixFilter->SetTransformParameterObject( trxParameterObject ); // supply a dummy object

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.

  this->m_HowToCite = "Klein S, Staring M, Murphy K, Viergever MA, Pluim JP. Elastix: a toolbox for intensity-based medical image registration. IEEE transactions on medical imaging. 2010 Jan;29(1):196-205";
}


template< int Dimensionality, class TPixel >
MonolithicTransformixComponent< Dimensionality, TPixel >::~MonolithicTransformixComponent()
{
}


template< int Dimensionality, class TPixel >
int
MonolithicTransformixComponent< Dimensionality, TPixel >::Accept( typename itkImageDomainFixedInterface< Dimensionality >::Pointer component )
{
  // TODO: transformix needs to have a parametermap in order to define its output image sizes for a proper functioning of the (itk) pipeline it is in.
  // Here we fill a parametermap with the sufficient info from the fixed image domain.
  // Currently, the fixed image domain is also part of the transformParameter map that will be set by elastix. Not sure what happens if these domains are not equal.
  

  auto fixedImageDomain = component->GetItkImageDomainFixed();

  auto size = fixedImageDomain->GetLargestPossibleRegion().GetSize();
  typename TransformixFilterType::ParameterValueVectorType sizeParameters;

  auto spacing = fixedImageDomain->GetSpacing();
  typename TransformixFilterType::ParameterValueVectorType spacingParameters;

  auto index = fixedImageDomain->GetLargestPossibleRegion().GetIndex();
  typename TransformixFilterType::ParameterValueVectorType indexParameters;

  auto origin = fixedImageDomain->GetOrigin();
  typename TransformixFilterType::ParameterValueVectorType originParameters;

  auto direction = fixedImageDomain->GetDirection();
  typename TransformixFilterType::ParameterValueVectorType directionParameters;

  for( unsigned int d = 0; d < Dimensionality; ++d )
  {
    sizeParameters.push_back( std::to_string( size[ d ] ) );
    spacingParameters.push_back( std::to_string( spacing[ d ] ) );
    indexParameters.push_back( std::to_string( index[ d ] ) );
    originParameters.push_back( std::to_string( origin[ d ] ) );
    for( unsigned int j = 0; j < Dimensionality; ++j)
    {
      directionParameters.push_back( std::to_string( direction(j,d) ) );
    }
  }

  elxParameterObjectPointer trxParameterObject = elxParameterObjectType::New();
  typename TransformixFilterType::ParameterMapType trxParameterMap = {
    { "FixedImageDimension", { std::to_string( Dimensionality ) } },
    { "MovingImageDimension", { std::to_string( Dimensionality ) } },
    { "FixedInternalImagePixelType", { "float" } },
    { "MovingInternalImagePixelType", { "float" } },
    { "Size", sizeParameters },
    { "Index", indexParameters },
    { "Spacing", spacingParameters },
    { "Origin", originParameters },
    { "Direction", directionParameters },
    { "UseDirectionCosines", { "true" } }
  };
  trxParameterObject->SetParameterMap( trxParameterMap );
  this->m_transformixFilter->SetTransformParameterObject( trxParameterObject );

  return 0;
}


template< int Dimensionality, class TPixel >
int
MonolithicTransformixComponent< Dimensionality, TPixel >::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_transformixFilter->SetMovingImage( movingImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
MonolithicTransformixComponent< Dimensionality, TPixel >::Accept( typename elastixTransformParameterObjectInterfaceType::Pointer component )
{
  //auto transformParameterObject = component->GetTransformParameterObject();
  // connect the itk pipeline
  // Due to the fact that elastixfilter returns a Null object we cannot use it as a pipeline
  //this->m_transformixFilter->SetTransformParameterObject(transformParameterObject);
  // Therefore store the interface for the Update call
  this->m_TransformParameterObjectInterface = component;
  return 0;
}

template< int Dimensionality, class TPixel >
typename MonolithicTransformixComponent< Dimensionality, TPixel >::ResultImageType::Pointer
MonolithicTransformixComponent< Dimensionality, TPixel >::GetItkImage()
{
  this->m_transformixFilter->ComputeDeformationFieldOff();
  return this->m_transformixFilter->GetOutput();
}

template< int Dimensionality, class TPixel >
typename MonolithicTransformixComponent< Dimensionality, TPixel >::ItkDisplacementFieldType::Pointer
MonolithicTransformixComponent< Dimensionality, TPixel >::GetItkDisplacementField()
{
  this->m_transformixFilter->ComputeDeformationFieldOn();
  return this->m_transformixFilter->GetOutputDeformationField();
}

template< int Dimensionality, class TPixel >
void
MonolithicTransformixComponent< Dimensionality, TPixel >::Update( void )
{
  // TODO currently, the pipeline with elastix and tranformix can only be created after the update of elastix
  this->m_transformixFilter->SetTransformParameterObject( this->m_TransformParameterObjectInterface->GetTransformParameterObject() );
}


template< int Dimensionality, class TPixel >
bool
MonolithicTransformixComponent< Dimensionality, TPixel >
::MeetsCriterion( const CriterionType & criterion )
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

  return meetsCriteria;
}

} //end namespace selx
