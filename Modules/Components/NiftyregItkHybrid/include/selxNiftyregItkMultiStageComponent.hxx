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

#include "selxNiftyregItkMultiStageComponent.h"
#include "selxCheckTemplateProperties.h"
#include "itkAffineTransform.h"

namespace selx
{
template< class InternalComputationValueType, int Dimensionality >
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::NiftyregItkMultiStageComponent( const std::string & name, LoggerImpl & logger ) : Superclass( name, logger )
{
  m_CompositeTransform = CompositeTransformType::New();

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< class InternalComputationValueType, int Dimensionality >
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >::~NiftyregItkMultiStageComponent()
{
}


template< class InternalComputationValueType, int Dimensionality >
int
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::Accept( typename MultiStageTransformInterface< InternalComputationValueType, Dimensionality >::Pointer other )
{
  // todo how do we organize the fixedtransforms?
  this->m_RegistrationStages.insert( { other->GetComponentName(), other } );
  return 1;
}

template< class InternalComputationValueType, int Dimensionality >
int
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::Accept(typename NiftyregAffineMatrixInterface::Pointer other)
{
	this->m_NiftyregAffineMatrixInterface = other;
	return 1;
}

template< class InternalComputationValueType, int Dimensionality >
int
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::Accept( UpdateInterface::Pointer other )
{
	this->m_UpdateInterfaces.insert({ other->GetComponentName(), other });
  return 0;
}


template< class InternalComputationValueType, int Dimensionality >
typename NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >::TransformType::Pointer
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >::GetItkTransform()
{
  return ( typename TransformType::Pointer ) this->m_CompositeTransform;
}


template< class InternalComputationValueType, int Dimensionality >
void
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >::Update()
{
  bool first = true;

  // Perform execution flow
  for( auto const & stageName : this->m_ExecutionOrder )
  {
    if (first)
    {
      m_UpdateInterfaces[stageName]->Update();
      // TODO check == null ?
      auto const & niftiMatrix = m_NiftyregAffineMatrixInterface->GetAffineNiftiMatrix()->m;
      
      using itkAffineTransformType = itk::AffineTransform<InternalComputationValueType, Dimensionality>;
      auto itkMatrix = typename itkAffineTransformType::MatrixType();
      auto itkTranslation = typename itkAffineTransformType::OffsetType();
      
      for ( unsigned int d1 = 0; d1 < Dimensionality; ++d1 )
      {
        for ( unsigned int d2 = 0; d2 < Dimensionality; ++d2 )
        {
          itkMatrix(d1,d2)=niftiMatrix[d1][d2];
        }
        itkTranslation[d1] = niftiMatrix[d1][3];
      }
      auto itkAffineTf = itkAffineTransformType::New();
      itkAffineTf->SetMatrix(itkMatrix);
      itkAffineTf->SetOffset(itkTranslation);
      this->m_CompositeTransform->AppendTransform( itkAffineTf );
      first=false;    
    }
    else // continue with standard itk multistage
    {
	    m_RegistrationStages[stageName]->SetMovingInitialTransform( this->m_CompositeTransform );
      m_UpdateInterfaces[stageName]->Update();
      this->m_CompositeTransform->AppendTransform( m_RegistrationStages[stageName]->GetItkTransform() );
    }
  }
}


template< class InternalComputationValueType, int Dimensionality >
bool
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::MeetsCriterion( const ComponentBase::CriterionType & criterion )
{
  bool hasUndefinedCriteria( false );

  auto status = CheckTemplateProperties( this->TemplateProperties(), criterion );
  if( status == CriterionStatus::Satisfied )
  {
    return true;
  }
  else if( status == CriterionStatus::Failed )
  {
    return false;
  } // else: CriterionStatus::Unknown

  if( criterion.first == "ExecutionOrder" ) //Supports this?
  {
    // should we check on non-repeating names?
    this->m_ExecutionOrder = criterion.second;
    return true;
  }
  return hasUndefinedCriteria;
}


template< class InternalComputationValueType, int Dimensionality >
bool
NiftyregItkMultiStageComponent< InternalComputationValueType, Dimensionality >
::ConnectionsSatisfied()
{
  // This function overrides the default behavior, in which all accepting interfaces must be set, by allowing the some interfaces not being set.
  // TODO: see I we can reduce the amount of code with helper (meta-)functions
  // A MultiStageTransformInterface connection is required
  if(! this->InterfaceAcceptor< MultiStageTransformInterface< InternalComputationValueType, Dimensionality >>::GetAccepted())
  {
    this->Error("Needs to have a MultiStageTransformInterface connected to it.",  this->m_Name);
    return false;
  }

  // A NiftyregAffineMatrixInterface connection is required
  if (!this->InterfaceAcceptor< NiftyregAffineMatrixInterface>::GetAccepted())
  {
    this->Error("Needs to have a NiftyregAffineMatrixInterface connected to it.",  this->m_Name);
    return false;
  }

  if ( 0 == this->m_ExecutionOrder.size())
  {
    this->Error("ExecutionOrder of {0} needs to be set with the names of the registration components. E.g. 'ExecutionOrder' : [ 'RegistrationMethod1', 'RegistrationMethod2' ] ", this->m_Name);
    return false;
  }
  // check if the component names in ExecutionOrder are connected both by MultiStageTransformInterface and UpdateInterface
  bool first = true;
  for( auto const & stageName : this->m_ExecutionOrder )
  {
    if (first)
    {
      if (m_NiftyregAffineMatrixInterface->GetComponentName()!=stageName)
      {
        this->Error("The first component '{0}' as named in the ExecutionOrder of '{1}' needs to be connected by  NiftyregAffineMatrixInterface. (Use named connections)", stageName, this->m_Name);
        return false;
      }
      first = false;
    }
	  else if (m_RegistrationStages.count(stageName)==0)
	  {
      this->Error("The component '{0}' as named in the ExecutionOrder of '{1}' needs to be connected by MultiStageTransformInterface. (Use named connections)", stageName, this->m_Name);
		  return false;
	  }
	  if (m_UpdateInterfaces.count(stageName) == 0)
	  {
      this->Error("The component '{0}' as named in the ExecutionOrder of '{1}' needs to be connected by UpdateInterface. (Use named connections)", stageName, this->m_Name);
		  return false;
	  }
  }

  if (m_RegistrationStages.size() > (m_ExecutionOrder.size()-1)) // assume 1 element in m_ExecutionOrder is from the NiftyregAffineMatrixInterface
  {
    this->Warning("'{0}' has unused MultiStageTransformInterface connections", this->m_Name);
  }
  if (m_UpdateInterfaces.size() > (m_ExecutionOrder.size()))
  {
    this->Warning("'{0}' has unused MultiStageTransformInterface connections", this->m_Name);
  }
  return true;
}
} //end namespace selx
