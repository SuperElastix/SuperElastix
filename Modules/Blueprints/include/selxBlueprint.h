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

#ifndef selxBlueprint_h
#define selxBlueprint_h

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "selxLogger.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace selx
{

class BlueprintImpl;

class Blueprint : public itk::DataObject
{
public:

  /** Standard ITK typedefs. */
  typedef Blueprint             Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( Self, itk::DataObject );

  typedef std::string                                      ParameterKeyType;
  typedef std::vector< std::string >                       ParameterValueType;
  typedef std::map< ParameterKeyType, ParameterValueType > ParameterMapType;
  typedef std::string                                      ComponentNameType;
  typedef std::vector< ComponentNameType >                 ComponentNamesType;

  /* m_Blueprint is initialized in the default constructor */
  Blueprint();
  ~Blueprint();

  /** The actual blueprint is a pimpled member variable */
  typedef std::unique_ptr< BlueprintImpl > BlueprintImplPointer;

  const BlueprintImpl & GetBlueprintImpl( void ) const; 

  bool SetComponent( ComponentNameType, ParameterMapType parameterMap );

  ParameterMapType GetComponent( ComponentNameType componentName ) const;

  bool DeleteComponent( ComponentNameType componentName );

  bool ComponentExists( ComponentNameType componentName ) const;

  // Returns a vector of the all Component names in the graph.
  ComponentNamesType GetComponentNames( void ) const;

  bool SetConnection( ComponentNameType upstream, ComponentNameType downstream, ParameterMapType parameterMap );

  ParameterMapType GetConnection( ComponentNameType upstream, ComponentNameType downstream ) const;

  bool DeleteConnection( ComponentNameType upstream, ComponentNameType downstream );

  bool ConnectionExists( ComponentNameType upstream, ComponentNameType downstream ) const;

  //std::unique_ptr<BlueprintImpl> Clone(BlueprintImpl const &other );

  // "functional" composition of blueprints is done by adding settings of other to this blueprint. Redefining/overwriting properties is not allowed and returns false.
  bool ComposeWith( Blueprint::ConstPointer other );

  // Returns a vector of the Component names at the incoming direction
  ComponentNamesType GetInputNames( const ComponentNameType name ) const;

  // Returns a vector of the Component names at the outgoing direction
  ComponentNamesType GetOutputNames( const ComponentNameType name ) const;

  // Write graphviz dot file
  void Write( const std::string filename );

  // Read json or XML file
  //void FromFile(const std::string& filename);

  void MergeFromFile(const std::string& filename);

  void SetLogger( Logger::Pointer logger );

private:

  // The order of the logger and the blueprint matters, since the lifetime of the logger should always exceed that of the blueprint.
  Logger::Pointer m_Logger;
  BlueprintImplPointer m_Blueprint;

};
}

#endif // #define selxBlueprint_h
