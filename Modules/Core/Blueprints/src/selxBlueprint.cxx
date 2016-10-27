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

#include "selxBlueprint.h"
#include "selxBlueprintImpl.h"

namespace selx {

  
Blueprint
::Blueprint( void ) : m_Pimple( new Blueprint::BlueprintImpl ) {}; 


Blueprint
::~Blueprint( void ) = default;


Blueprint::ParameterMapType
Blueprint
::GetComponent( ComponentNameType name ) const
{
  return this->m_Pimple->GetComponent( name );
}


bool
Blueprint
::SetComponent( ComponentNameType name, ParameterMapType parameterMap )
{
  return this->m_Pimple->SetComponent( name, parameterMap );
}


Blueprint::ComponentNamesType
Blueprint::GetComponentNames( void ) const
{
  return this->m_Pimple->GetComponentNames();
}


Blueprint::ParameterMapType
Blueprint
::GetConnection( ComponentNameType upstream, ComponentNameType downstream ) const
{
  return this->m_Pimple->GetConnection( upstream, downstream );
}


bool
Blueprint
::SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap )
{
  return this->m_Pimple->SetConnection( upstream, downstream, parameterMap );
}


bool
Blueprint
::DeleteConnection( ComponentNameType upstream, ComponentNameType downstream )
{
  return this->m_Pimple->DeleteConnection( upstream, downstream );
}


bool
Blueprint
::ComponentExists( ComponentNameType componentName ) const
{
  return this->m_Pimple->ComponentExists( componentName );
}


bool
Blueprint
::ConnectionExists( ComponentNameType upstream, ComponentNameType downstream ) const
{
  return this->m_Pimple->ConnectionExists( upstream, downstream );
}


Blueprint::ComponentNamesType
Blueprint
::GetOutputNames( const ComponentNameType name ) const
{
  return this->m_Pimple->GetOutputNames( name );
}


Blueprint::ComponentNamesType
Blueprint
::GetInputNames( const ComponentNameType name ) const
{
  return this->m_Pimple->GetInputNames( name );
}


void
Blueprint
::Write( const std::string filename )
{
  this->m_Pimple->Write( filename );
}

} // namespace selx