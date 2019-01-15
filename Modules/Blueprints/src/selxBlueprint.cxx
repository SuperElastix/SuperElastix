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

namespace selx
{

Blueprint
::Blueprint() : 
  m_Logger( Logger::New() ), // Create default logger which redirects to std::cout
  m_BlueprintImpl( new BlueprintImpl( this->m_Logger->GetLoggerImpl() ) ) 
{
}

// This class uses the pimpl idiom and therefore needs to implement the destructor explicitly: Effective Modern C++, Scott Meyers, item 22
Blueprint
::~Blueprint()
{
}

const BlueprintImpl &
Blueprint
::GetBlueprintImpl( void ) const
{
  return *this->m_BlueprintImpl;
}


bool
Blueprint
::SetComponent( ComponentNameType name, ParameterMapType parameterMap )
{
  this->Modified();
  return this->m_BlueprintImpl->SetComponent( name, parameterMap );
}


Blueprint::ParameterMapType
Blueprint
::GetComponent( ComponentNameType componentName ) const
{
  return this->m_BlueprintImpl->GetComponent( componentName );
}


bool
Blueprint
::DeleteComponent( ComponentNameType componentName )
{
  this->Modified();
  return this->m_BlueprintImpl->DeleteComponent( componentName );
}


Blueprint::ComponentNamesType
Blueprint::GetComponentNames( void ) const
{
  return this->m_BlueprintImpl->GetComponentNames();
}


bool
Blueprint
::SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap, ConnectionNameType name )
{
  this->Modified();
  return this->m_BlueprintImpl->SetConnection( upstream, downstream, parameterMap, name );
}


Blueprint::ParameterMapType
Blueprint
::GetConnection( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name ) const
{
  return this->m_BlueprintImpl->GetConnection( upstream, downstream, name );
}


bool
Blueprint
::DeleteConnection( ComponentNameType upstream, ComponentNameType downstream, ConnectionNameType name )
{
  this->Modified();
  return this->m_BlueprintImpl->DeleteConnection( upstream, downstream, name );
}


bool
Blueprint
::ComponentExists( ComponentNameType componentName ) const
{
  return this->m_BlueprintImpl->ComponentExists( componentName );
}


bool
Blueprint
::ConnectionExists( ComponentNameType upstream, ComponentNameType downstream,  ConnectionNameType name  ) const
{
  return this->m_BlueprintImpl->ConnectionExists( upstream, downstream, name );
}


bool
Blueprint
::ComposeWith( const Blueprint * other)
{
  this->Modified();
  return this->m_BlueprintImpl->ComposeWith( other->GetBlueprintImpl() );
}


Blueprint::ComponentNamesType
Blueprint
::GetOutputNames( const ComponentNameType name ) const
{
  return this->m_BlueprintImpl->GetOutputNames( name );
}


Blueprint::ComponentNamesType
Blueprint
::GetInputNames( const ComponentNameType name ) const
{
  return this->m_BlueprintImpl->GetInputNames( name );
}


void
Blueprint
::Write( const std::string filename )
{
  this->m_BlueprintImpl->Write( filename );
}

void
Blueprint
::MergeFromFile( const std::string& filename )
{
  this->m_BlueprintImpl->MergeFromFile( filename );
}

void
Blueprint
::SetLogger( Logger::Pointer logger )
{
  this->m_Logger = logger;
  this->m_BlueprintImpl->SetLoggerImpl( logger->GetLoggerImpl() );
}


} // namespace selx
