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

namespace selx
{
  namespace keys
  {
    const char * const NameOfInterface = "NameOfInterface"; // Each Interface has a sting name 
    const char * const HasProvidingInterface = "HasProvidingInterface"; // Checks component (base class) if the interface is present
    const char * const HasAcceptingInterface = "HasAcceptingInterface"; // Checks component (base class) if the interface is present 
    const char * const SourceInterface = "SourceInterface"; // Special interface that connects to the outside of the SuperElastixFilter 
    const char * const SinkInterface = "SinkInterface"; // Special interface that connects to the outside of the SuperElastixFilter 
    
  }
}