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

/** Helper definitions for common cstring key values to avoid typos*/

// In SuperElastix we use cstring keys to communicate between components. The
// advantage is that user defined cstring-based configurations can directly talk to the core.
// In the core we could type these names directly as literal cstrings, but this is sensitive to typos.
// By these definitions tools such as visual assist will auto-complete when typing keys::HasP...

#include "selxCheckTemplateProperties.h"

#include <sstream>
#include <exception>

namespace selx
{

CriterionStatus
CheckTemplateProperties( const std::map< std::string, std::string > templateProperties,
  const std::pair< std::string, std::vector< std::string >> criterion )
{
  if( templateProperties.count( criterion.first ) == 1 ) // e.g. is "Dimensionality" a template property? Or is NameOfClass queried?
  {
    if( criterion.second.size() != 1 )  // criteria can be of format: "keystring": ["value1", value2","value3"], but for templateproperties only 1 value is allowed.
    {
      std::stringstream msg;
      msg << "The criterion " << criterion.first << " may have only 1 value";
      throw std::runtime_error( msg.str() );
      //itkExceptionMacro("The criterion " << criterion.first << " may have only 1 value");
    }
    for( auto const & criterionValue : criterion.second )
    {
      if( criterionValue != templateProperties.at( criterion.first ) )
      {
        return CriterionStatus::Failed;
      }
    }
    return CriterionStatus::Satisfied;
  }
  return CriterionStatus::Unknown;
}
}

