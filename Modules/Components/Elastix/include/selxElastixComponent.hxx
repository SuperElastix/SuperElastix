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

#include "selxElastixComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
ElastixComponent< Dimensionality, TPixel >::ElastixComponent( const std::string & name ) : SuperElastixComponent( name )
{
  m_elastixFilter     = ElastixFilterType::New();
  m_transformixFilter = TransformixFilterType::New();

  // TODO: Due to some issues with Criteria being propagated as elastix settings, I need to empty the selxparameterObject.
  elxParameterObjectPointer elxParameterObject = elxParameterObjectType::New();
  typename elxParameterObjectType::ParameterMapType defaultParameters = elxParameterObject->GetDefaultParameterMap( "rigid" );

  elxParameterObject->SetParameterMap( defaultParameters );
  m_elastixFilter->SetParameterObject( elxParameterObject );
  m_elastixFilter->LogToConsoleOn();
  m_elastixFilter->LogToFileOff();
  m_elastixFilter->SetOutputDirectory( "." );

  m_transformixFilter->ComputeDeformationFieldOn();
  m_transformixFilter->LogToConsoleOn();
  m_transformixFilter->LogToFileOff();
  m_transformixFilter->SetOutputDirectory( "." );

  //TODO m_elastixFilter returns a nullptr GetTransformParameterObject instead of a valid object. However, we need this object to satisfy the input conditions of m_transformixFilter
  //m_transformixFilter->SetTransformParameterObject(m_elastixFilter->GetTransformParameterObject());
  m_transformixFilter->SetTransformParameterObject( elxParameterObject ); // supply a dummy object

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< int Dimensionality, class TPixel >
ElastixComponent< Dimensionality, TPixel >::~ElastixComponent()
{
}


template< int Dimensionality, class TPixel >
int
ElastixComponent< Dimensionality, TPixel >::Set( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_elastixFilter->SetFixedImage( fixedImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
ElastixComponent< Dimensionality, TPixel >::Set( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_elastixFilter->SetMovingImage( movingImage );
  // In the current transformix filter an input image is required even if we want a deformation field only.
  this->m_transformixFilter->SetMovingImage( movingImage );
  return 0;
}


//Since elastixFilter is not a true itkfilter we cannot use itkImageSourceInterface (yet)
/*template<int Dimensionality, class TPixel>
typename ElastixComponent< Dimensionality, TPixel>::ItkImageSourcePointer ElastixComponent< Dimensionality, TPixel>::GetItkImageSource()
{

  return (ItkImageSourcePointer) this->m_theItkFilter;
}
*/

template< int Dimensionality, class TPixel >
typename ElastixComponent< Dimensionality, TPixel >::ItkImagePointer
ElastixComponent< Dimensionality, TPixel >::GetItkImage()
{
  return this->m_transformixFilter->GetOutput();
}


template< int Dimensionality, class TPixel >
void
ElastixComponent< Dimensionality, TPixel >::RunRegistration( void )
{
  // TODO currently, the pipeline with elastix and tranformix can only be created after the update of elastix
  this->m_elastixFilter->Update();
  this->m_transformixFilter->SetTransformParameterObject( this->m_elastixFilter->GetTransformParameterObject() );
  this->m_transformixFilter->Update();
}


template< int Dimensionality, class TPixel >
bool
ElastixComponent< Dimensionality, TPixel >
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

  else if( criterion.first == "RegistrationPreset" )  //Supports this?
  {
    // Temporary solution: RegistrationPreset: rigid, nonrigid, etc overwrite the current selxparameterObject.
    // Warning: the order of Criteria matters, since selxparameterObject may be overwritten
    // Warning: this probably fails because the Criteria map entries are processed in arbitrary order.

    elxParameterObjectPointer elxParameterObject = elxParameterObjectType::New();

    meetsCriteria = true;
    for( auto const & presetName : criterion.second )  // auto&& preferred?
    {
      typename elxParameterObjectType::ParameterMapType presetParameters = elxParameterObject->GetDefaultParameterMap( presetName );
      elxParameterObject->SetParameterMap( presetParameters );

      try
      {
        this->m_elastixFilter->SetParameterObject( elxParameterObject );
      }
      catch( itk::ExceptionObject & err )
      {
        std::cout << err;
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  else
  {
    // temporary solution: pass all SuperElastixComponent parameters as is to elastix. This should be defined in deeper hierarchy of the criteria, but for now we have a flat mapping only.
    elxParameterObjectPointer elxParameterObject = this->m_elastixFilter->GetParameterObject();
    typename elxParameterObjectType::ParameterMapType newParameterMap = elxParameterObject->GetParameterMap( 0 ); //copy const paramtermap to a non const map
    newParameterMap[ criterion.first ]                                = criterion.second;                         //overwrite element
    elxParameterObjectPointer newParameterObject = elxParameterObjectType::New();
    newParameterObject->SetParameterMap( newParameterMap );
    this->m_elastixFilter->SetParameterObject( newParameterObject );
    meetsCriteria = true;
  }
  return meetsCriteria;
}
} //end namespace selx
