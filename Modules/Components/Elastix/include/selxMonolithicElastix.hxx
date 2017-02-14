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

#include "selxMonolithicElastix.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
MonolithicElastixComponent< Dimensionality, TPixel >::MonolithicElastixComponent(const std::string & name) : SuperElastixComponent(name)
{
  m_elastixFilter = ElastixFilterType::New();

  // TODO: Due to some issues with Criteria being propagated as elastix settings, I need to empty the selxparameterObject.
  elxParameterObjectPointer elxParameterObject = elxParameterObjectType::New();
  typename elxParameterObjectType::ParameterMapType defaultParameters = elxParameterObject->GetDefaultParameterMap( "rigid" );

  elxParameterObject->SetParameterMap( defaultParameters );
  m_elastixFilter->SetParameterObject( elxParameterObject );
  m_elastixFilter->LogToConsoleOn();
  m_elastixFilter->LogToFileOff();
  m_elastixFilter->SetOutputDirectory( "." );
}


template< int Dimensionality, class TPixel >
MonolithicElastixComponent< Dimensionality, TPixel >::~MonolithicElastixComponent()
{
}


template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Set( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_elastixFilter->SetFixedImage( fixedImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Set( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_elastixFilter->SetMovingImage( movingImage );
  return 0;
}


template< int Dimensionality, class TPixel >
typename MonolithicElastixComponent< Dimensionality, TPixel >::ItkImagePointer
MonolithicElastixComponent< Dimensionality, TPixel >::GetItkImage()
{
  return this->m_elastixFilter->GetOutput();
}


template< int Dimensionality, class TPixel >
typename MonolithicElastixComponent< Dimensionality,
TPixel >::elastixTransformParameterObject * MonolithicElastixComponent< Dimensionality, TPixel >::GetTransformParameterObject()
{
  return this->m_elastixFilter->GetTransformParameterObject();
}

template< int Dimensionality, class TPixel >
void
MonolithicElastixComponent< Dimensionality, TPixel >::RunRegistration( void )
{
  this->m_elastixFilter->Update();
}


template< int Dimensionality, class TPixel >
bool
MonolithicElastixComponent< Dimensionality, TPixel >
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
    // temporary solution: pass all SuperMonolithicElastixComponent parameters as is to elastix. This should be defined in deeper hierarchy of the criteria, but for now we have a flat mapping only.
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
