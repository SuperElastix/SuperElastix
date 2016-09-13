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

#ifndef selxLogComponent_cxx
#define selxLogComponent_cxx

#include "selxLogComponent.h"

namespace selx
{

LogComponent
::LogComponent( void )
{
  // this->m_LogLevel = boost::log::trivial::severity_level::info;
}

std::stringstream
LogComponent
::Log( boost::log::trivial::severity_level severity_level )
{
  return BOOST_LOG_TRIVIAL( ::boost::log::keywords::severity = severity_level );
}

bool
LogComponent
::MeetsCriterion( const CriterionType & criterion )
{
  bool meetsCriteria( false );
  if( criterion.first == "LogLevel" )
  {
    meetsCriteria = true;
    //for( auto const & criterionValue : criterion.second )  // auto&& preferred?
    //{
      //if( criterionValue != "INFO" )   // e.g. "GradientDescent", "SupportsSparseSamples
      //{
      //  meetsCriteria = false;
      //}
    //}
  }

  return meetsCriteria;
}

}

#endif // selxLogComponent_cxx
