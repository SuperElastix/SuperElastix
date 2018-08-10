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

#include "selxMonolithicElastixComponent.h"
#include "selxCheckTemplateProperties.h"

namespace selx
{
template< int Dimensionality, class TPixel >
MonolithicElastixComponent< Dimensionality, TPixel >::MonolithicElastixComponent( const std::string & name,
  LoggerImpl & logger ) : Superclass( name, logger )
{
  m_ParameterObject = ParameterObjectType::New();
  m_ElastixFilter = ElastixFilterType::New();
  m_ElastixFilter->LogToConsoleOn();
  m_ElastixFilter->LogToFileOff();
  m_ElastixFilter->SetOutputDirectory( "." );

  this->m_HowToCite = "Klein S, Staring M, Murphy K, Viergever MA, Pluim JP. Elastix: a toolbox for intensity-based medical image registration. IEEE transactions on medical imaging. 2010 Jan;29(1):196-205";
}


template< int Dimensionality, class TPixel >
MonolithicElastixComponent< Dimensionality, TPixel >::~MonolithicElastixComponent()
{
}


template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Accept( typename itkImageFixedInterface< Dimensionality, TPixel >::Pointer component )
{
  auto fixedImage = component->GetItkImageFixed();
  // connect the itk pipeline
  this->m_ElastixFilter->SetFixedImage( fixedImage );
  return 0;
}


template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Accept( typename itkImageMovingInterface< Dimensionality, TPixel >::Pointer component )
{
  auto movingImage = component->GetItkImageMoving();
  // connect the itk pipeline
  this->m_ElastixFilter->SetMovingImage( movingImage );
  return 0;
}

template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Accept(typename itkImageFixedMaskInterface< Dimensionality, unsigned char >::Pointer component)
{
  auto fixedMaskImage = component->GetItkImageFixedMask();
  // connect the itk pipeline
  this->m_ElastixFilter->SetFixedMask(fixedMaskImage);
  return 0;
}


template< int Dimensionality, class TPixel >
int
MonolithicElastixComponent< Dimensionality, TPixel >::Accept(typename itkImageMovingMaskInterface< Dimensionality, unsigned char >::Pointer component)
{
  auto movingMaskImage = component->GetItkImageMovingMask();
  // connect the itk pipeline
  this->m_ElastixFilter->SetMovingMask(movingMaskImage);
  return 0;
}

template< int Dimensionality, class TPixel >
typename MonolithicElastixComponent< Dimensionality, TPixel >::ItkImagePointer
MonolithicElastixComponent< Dimensionality, TPixel >::GetItkImage()
{
  this->m_ElastixFilter->SetParameterObject(this->m_ParameterObject);
  return this->m_ElastixFilter->GetOutput();
}


template< int Dimensionality, class TPixel >
typename MonolithicElastixComponent< Dimensionality,
TPixel >::elastixTransformParameterObject * MonolithicElastixComponent< Dimensionality, TPixel >::GetTransformParameterObject()
{
  return this->m_ElastixFilter->GetTransformParameterObject();
}

template< int Dimensionality, class TPixel >
void
MonolithicElastixComponent< Dimensionality, TPixel >::Update( void )
{
  this->m_ElastixFilter->SetParameterObject(this->m_ParameterObject);
  this->m_ElastixFilter->Update();
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

  // Check if this is a parameter map setting
  if(strncmp(criterion.first.c_str(), "ParameterMap", 12) == 0) {
    // Add to parameter map
    std::stringstream ss;
    ss << criterion.first.substr(12, 13);
    unsigned int parameterMapIndex = 0;
    ss >> parameterMapIndex;

    if(!ss.good()) {
      this->m_Logger.Log(LogLevel::ERR, "Expected parameter map number, got {}.", parameterMapIndex);
      return false;
    }

    // Create new parameter map
    while(this->m_ParameterObject->GetParameterMap().size() <= parameterMapIndex) {
      this->m_ParameterObject->AddParameterMap(ParameterObjectType::ParameterMapType());
    }

    std::string parameterKey = criterion.first.substr(13);

    if(parameterKey == "Preset") {
      this->m_ParameterObject->SetParameterMap(parameterMapIndex,
                                               this->m_ParameterObject->GetDefaultParameterMap(criterion.second[0]));
    }

    this->m_ParameterObject->SetParameter(parameterMapIndex, parameterKey, criterion.second);

    return true;
  }

  return meetsCriteria;
}

template< int Dimensionality, class TPixel >
bool
MonolithicElastixComponent< Dimensionality, TPixel >
::ConnectionsSatisfied()
{
  // This function overrides the default behavior, in which all accepting interfaces must be set.
  // Only Fixed and Moving images are required
  // TODO: see I we can reduce the amount of code with helper (meta-)functions
  if (!this->InterfaceAcceptor< itkImageFixedInterface< Dimensionality, TPixel >>::GetAccepted())
  {
    return false;
  }
  if (!this->InterfaceAcceptor< itkImageMovingInterface< Dimensionality, TPixel >>::GetAccepted())
  {
    return false;
  }
  
  return true;
}

} //end namespace selx
