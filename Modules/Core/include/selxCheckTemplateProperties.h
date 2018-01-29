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

#ifndef selxCheckTemplateProperties_h
#define selxCheckTemplateProperties_h

#include <map>
#include <string>
#include <vector>

namespace selx
{
enum class CriterionStatus { Satisfied, Failed, Unknown };

CriterionStatus
CheckTemplateProperties( const std::map< std::string, std::string > templateProperties,
  const std::pair< std::string, std::vector< std::string >> criterion );
}
#endif //selxCheckTemplateProperties_h
