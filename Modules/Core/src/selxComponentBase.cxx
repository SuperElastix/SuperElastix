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

#include "selxComponentBase.h"

namespace selx
{
// TODO delete this constructor
ComponentBase::ComponentBase() : m_Name( "undefined" ), m_Logger( *( new LoggerImpl() ) )
{
}

ComponentBase::ComponentBase(const std::string & name, LoggerImpl & logger) : m_Logger(logger), m_Name( name )
{
}


//const std::map< std::string, std::string > ComponentBase::TemplateProperties()
// { return{}; }
} // end namespace selx

unsigned short Test_to_see_whether_warning_messages_are_displayed_at_CDash = 1.79769e+308;
void* Test_to_see_whether_error_messages_are_displayed_at_CDash = 42;
#error "Test to see whether error messages are displayed at http://my.cdash.org/index.php?project=SuperElastix"