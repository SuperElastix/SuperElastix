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

#include "selxItkGradientDescentOptimizerv4Component.h"
#include <boost/lexical_cast.hpp>
#include "selxPodString.h"
#include "selxStringConverter.h"

namespace selx
{
template< class InternalComputationValueType >
ItkGradientDescentOptimizerv4Component< InternalComputationValueType >::ItkGradientDescentOptimizerv4Component( const std::string & name,
  LoggerImpl & logger ) :
  Superclass( name, logger )
{
  m_Optimizer = GradientDescentOptimizerv4Type::New();
  m_Optimizer->SetNumberOfIterations( 100 );
  m_Optimizer->SetLearningRate( 1.0 );
  m_Optimizer->SetDoEstimateLearningRateAtEachIteration(false);
  m_Optimizer->SetDoEstimateLearningRateOnce(true);
  m_Optimizer->SetMaximumStepSizeInPhysicalUnits( 1.0 );

  //TODO: instantiating the filter in the constructor might be heavy for the use in component selector factory, since all components of the database are created during the selection process.
  // we could choose to keep the component light weighted (for checking criteria such as names and connections) until the settings are passed to the filter, but this requires an additional initialization step.
}


template< class InternalComputationValueType >
ItkGradientDescentOptimizerv4Component< InternalComputationValueType >::~ItkGradientDescentOptimizerv4Component()
{
}


template< class InternalComputationValueType >
typename ItkGradientDescentOptimizerv4Component< InternalComputationValueType >::Optimizerv4Pointer
ItkGradientDescentOptimizerv4Component< InternalComputationValueType >::GetItkOptimizerv4()
{
  return (Optimizerv4Pointer)this->m_Optimizer;
}


template< class InternalComputationValueType >
bool
ItkGradientDescentOptimizerv4Component< InternalComputationValueType >
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

  if( criterion.first == "NumberOfIterations" || criterion.first == "MaximumNumberOfIterations" )
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
        this->m_Optimizer->SetNumberOfIterations( std::stoi( criterionValue ) );
        meetsCriteria = true;
      }
      catch( itk::ExceptionObject & itkNotUsed(err) )
      {
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "LearningRate" ) //Supports this?
  {
    if( criterion.second.size() != 1 )
    {
      meetsCriteria = false;
    }
    else
    {
      auto const & criterionValue = *criterion.second.begin();
      try
      {
        this->m_Optimizer->SetLearningRate( boost::lexical_cast< InternalComputationValueType >( criterionValue ) );
        meetsCriteria = true;
      }
      catch( itk::ExceptionObject & itkNotUsed(err) ) // TODO: should catch(const bad_lexical_cast &) too
      {
        //TODO log the error message?
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "EstimateLearningRate" ) //Supports this?
  {
    if( criterion.second.size() != 1 )
    {
      meetsCriteria = false;
    }
    else
    {
      auto const & criterionValue = *criterion.second.begin();
      if( criterionValue == "Once" )
      {
        this->m_Optimizer->SetDoEstimateLearningRateOnce(true);
        this->m_Optimizer->SetDoEstimateLearningRateAtEachIteration(false);
        meetsCriteria = true;
      }
      else if( criterionValue == "EveryIteration" )
      {
        this->m_Optimizer->SetDoEstimateLearningRateOnce(false);
        this->m_Optimizer->SetDoEstimateLearningRateAtEachIteration(true);
        meetsCriteria = true;
      }
      else if( criterionValue == "False" ) {
        this->m_Optimizer->SetDoEstimateLearningRateOnce(false);
        this->m_Optimizer->SetDoEstimateLearningRateAtEachIteration(false);
        meetsCriteria = true;
      }
      else {
        meetsCriteria = false;
      }
    }
  }
  else if( criterion.first == "EstimateScales" ) //Supports this?
  {
    if( criterion.second.size() != 1 )
    {
      meetsCriteria = false;
    }
    else
    {
      bool criterionValue = false;
      meetsCriteria = StringConverter::Convert(*criterion.second.begin(), criterionValue);
      if(meetsCriteria)
      {
        this->m_Optimizer->SetDoEstimateScales(criterionValue);
      }
    }
  }
  else if( criterion.first == "MaximumStepSizeInPhysicalUnits" ) {
    if( criterion.second.size() != 1 )
    {
      meetsCriteria = false;
    }
    else
    {
      this->m_Optimizer->SetMaximumStepSizeInPhysicalUnits( std::stof( criterion.second[0] ) );
      meetsCriteria = true;
    }
  }
  return meetsCriteria;
}
} //end namespace selx
