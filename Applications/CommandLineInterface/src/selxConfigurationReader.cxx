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

#ifndef ConfigurationReader_cxx
#define ConfigurationReader_cxx

#include "selxConfigurationReader.h"
#include "selxLoggerImpl.h"
#include <ostream>

namespace selx
{
ConfigurationReader::ParameterValueType
ConfigurationReader::VectorizeValues( ComponentOrConnectionTreeType & componentOrConnectionTree )
{
  std::string        propertySingleValue = componentOrConnectionTree.data();
  ParameterValueType propertyMultiValue;
  for( auto const & value : componentOrConnectionTree )
  {
    propertyMultiValue.push_back( value.second.data() );
  }
  if( propertyMultiValue.size() > 0 )
  {
    if( propertySingleValue != "" )
    {
      throw std::invalid_argument( "XML tree should have either 1 unnamed element or multiple named properties" );
    }
  }
  else
  {
    propertyMultiValue.push_back( propertySingleValue );
  }
  return propertyMultiValue;
}


Blueprint::Pointer
ConfigurationReader::FromFile(const PathType & fileName)
{
  LoggerImpl logger;

  logger.Log( LogLevel::INF, "Loading {0} ... ");
  auto propertyTree = ReadPropertyTree(fileName);
  logger.Log( LogLevel::INF, "Loading {0} ... Done");

  logger.Log( LogLevel::INF, "Checking {0} for include files ... ", fileName);
  auto includesList = FindIncludes(propertyTree);

  if (includesList.size() == 0)
  {
    logger.Log( LogLevel::INF, "Checking {0} for include files ... Done. No include files specified.", fileName);
    return FromPropertyTree(propertyTree);
  }
  else
  {
    Blueprint::Pointer baseBlueprint = Blueprint::New();
    for (auto const & includePath : includesList)
    {
      logger.Log( LogLevel::INF, "Including file {0} ... ", includePath);
      baseBlueprint->ComposeWith(FromFile(includePath).GetPointer());
      logger.Log( LogLevel::INF, "Including include file {0} ... Done", includePath);
    }
    baseBlueprint->ComposeWith(FromPropertyTree(propertyTree).GetPointer());
    logger.Log( LogLevel::INF, "Checking {0} for include files ... Done");
    return baseBlueprint;
  }
}

void
ConfigurationReader::MergeFromFile(Blueprint::Pointer blueprint, const PathType & fileName)
{
  LoggerImpl logger;

  logger.Log( LogLevel::INF, "Loading {0} ... ", fileName);
  auto propertyTree = ReadPropertyTree(fileName);
  logger.Log( LogLevel::INF, "Loading {0} ... Done", fileName);

  logger.Log( LogLevel::INF, "Checking {0} for include files ... ", fileName);
  auto includesList = FindIncludes(propertyTree);

  if (includesList.size() > 0)
  {
    for (auto const & includePath : includesList)
    {
      logger.Log( LogLevel::INF, "Including file {0} ... ", includePath);
      MergeFromFile(blueprint, includePath);
    }
  }
  logger.Log( LogLevel::INF, "Checking {0} for include files ... done", fileName);
  MergeProperties(blueprint, propertyTree);
  return;
}

ConfigurationReader::PropertyTreeType
ConfigurationReader::ReadPropertyTree(const PathType & filename)
{
  // Create empty property tree object
  PropertyTreeType propertyTree;
  if (filename.extension() == ".xml")
  {
    read_xml(filename.string(), propertyTree, boost::property_tree::xml_parser::trim_whitespace);
  }
  else if (filename.extension() == ".json")
  {
    read_json(filename.string(), propertyTree);
  }
  else
  {
    throw std::invalid_argument("Configuration file requires extension .xml or .json");
  }

  return propertyTree;
}

ConfigurationReader::PathsType 
ConfigurationReader::FindIncludes(const PropertyTreeType & propertyTree)
{
  PathsType paths;
  bool FoundIncludes = false;
  BOOST_FOREACH(const PropertyTreeType::value_type & v, propertyTree.equal_range("Include"))
  {
    if (FoundIncludes)
    {
      std::runtime_error("Only 1 listing of Includes is allowed per Blueprint file");
    }

    auto const pathsStrings = VectorizeValues(v.second);
    // convert vector of strings to list of boost::path-s
    paths.resize(pathsStrings.size());
    std::transform(pathsStrings.begin(), pathsStrings.end(), paths.begin(),
      [](std::string p) { return PathType(p); });
    FoundIncludes = true;
  }
  return paths;
}


Blueprint::Pointer
ConfigurationReader::FromPropertyTree( const PropertyTreeType & pt )
{
  LoggerImpl logger;
  Blueprint::Pointer blueprint = Blueprint::New();

  BOOST_FOREACH( const PropertyTreeType::value_type & v, pt.equal_range( "Component" ) )
  {
    std::string      componentName;
    ParameterMapType componentPropertyMap;
    for( auto const & elm : v.second )
    {
      const std::string & componentKey = elm.first;
      if( componentKey == "Name" )
      {
        componentName = elm.second.data();
        continue;
      }

      ParameterValueType propertyMultiValue = VectorizeValues( elm.second );
      std::string        propertyKey        = elm.first;
      componentPropertyMap[ propertyKey ] = propertyMultiValue;
    }

    blueprint->SetComponent( componentName, componentPropertyMap );
  }

  BOOST_FOREACH( const PropertyTreeType::value_type & v, pt.equal_range( "Connection" ) )
  {
    std::string connectionName = v.second.data();
    if( connectionName != "" )
    {
      // TODO: Why is it ignored?
      logger.Log( LogLevel::WRN, "Connection {0} is ignored.", connectionName);
    }
    std::string      outName;
    std::string      inName;
    ParameterMapType componentPropertyMap;

    for( auto const & elm : v.second )
    {
      const std::string & connectionKey = elm.first;

      if( connectionKey == "Out" )
      {
        outName = elm.second.data();
        continue;
      }
      else if( connectionKey == "In" )
      {
        inName = elm.second.data();
        continue;
      }
      else if( connectionKey == "Name" )
      {
        logger.Log( LogLevel::WRN, "Connections with key 'Name' are ignored.");
        continue;
      }
      else
      {
        ParameterValueType propertyMultiValue = VectorizeValues( elm.second );
        std::string        propertyKey        = elm.first;
        componentPropertyMap[ propertyKey ] = propertyMultiValue;
      }
    }

    blueprint->SetConnection( outName, inName, componentPropertyMap );
  }
  return blueprint;
}

void
ConfigurationReader::MergeProperties(Blueprint::Pointer blueprint, const PropertyTreeType & pt)
{
  LoggerImpl logger;
  BOOST_FOREACH(const PropertyTreeType::value_type & v, pt.equal_range("Component"))
  {
    std::string      componentName;
    ParameterMapType newProperties;
    for (auto const & elm : v.second)
    {
      const std::string & componentKey = elm.first;
      if (componentKey == "Name")
      {
        componentName = elm.second.data();
        continue;
      }

      ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
      std::string        propertyKey = elm.first;
      newProperties[propertyKey] = propertyMultiValue;
    }

    // Merge newProperies into current blueprint properties
    // Does blueprint use component with a name that already exists?
    if (blueprint->ComponentExists(componentName))
    {
      // Component exists, check if properties can be merged
      auto currentProperties = blueprint->GetComponent(componentName);

      for (auto const & othersEntry : newProperties)
      {
        // Does other use a property key that already exists in this component?
        if (currentProperties.count(othersEntry.first))
        {
          auto && ownValues = currentProperties[othersEntry.first];
          auto && otherValues = othersEntry.second;
          // Are the property values equal?
          if (ownValues.size() != otherValues.size())
          {
            // No, based on the number of values we see that it is different. Blueprints cannot be Composed
            throw std::invalid_argument("Merging blueprints failed : Component properties cannot be redefined");
          }
          else
          {
            ParameterValueType::const_iterator ownValue;
            ParameterValueType::const_iterator otherValue;
            for (ownValue = ownValues.begin(), otherValue = otherValues.begin(); ownValue != ownValues.end(); ++ownValue, ++otherValue)
            {
              if (*otherValue != *ownValue)
              {
                // No, at least one value is different. Blueprints cannot be Composed
                throw std::invalid_argument("Merging blueprints failed: Component properties cannot be redefined");
              }
            }
          }
        }
        else
        {
          // Property key doesn't exist yet, add entry to this component
          auto ownProperties = blueprint->GetComponent(componentName);
          ownProperties[othersEntry.first] = othersEntry.second;
          blueprint->SetComponent(componentName, ownProperties);
        }
      }
    }
    else
    {
      // Create Component and with the new properties
      blueprint->SetComponent(componentName, newProperties);
    }
  }

  BOOST_FOREACH(const PropertyTreeType::value_type & v, pt.equal_range("Connection"))
  {
    std::string connectionName = v.second.data();
    if (connectionName != "")
    {
      logger.Log( LogLevel::WRN, "Connection {0} is ignored.", connectionName);
    }
    std::string      outName;
    std::string      inName;
    ParameterMapType newProperties;

    for (auto const & elm : v.second)
    {
      const std::string & connectionKey = elm.first;

      if (connectionKey == "Out")
      {
        outName = elm.second.data();
        continue;
      }
      else if (connectionKey == "In")
      {
        inName = elm.second.data();
        continue;
      }
      else if (connectionKey == "Name")
      {
        logger.Log( LogLevel::WRN, "Connections with key 'Name' are ignored.");
        continue;
      }
      else
      {
        ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
        std::string        propertyKey = elm.first;
        newProperties[propertyKey] = propertyMultiValue;
      }
    }

  // Does the blueprint have a connection that already exists?
    if (blueprint->ConnectionExists(outName, inName))
    {
      // Connection exists, check if properties can be merged
      auto ownProperties = blueprint->GetConnection(outName, inName);

      for (auto const & othersEntry : newProperties)
      {
        // Does newProperties use a key that already exists in this component?
        if (ownProperties.count(othersEntry.first))
        {
          auto && ownValues = ownProperties[othersEntry.first];
          auto && otherValues = othersEntry.second;
          // Are the property values equal?
          if (ownValues.size() != otherValues.size())
          {
            // No, based on the number of values we see that it is different. Blueprints cannot be Composed
            throw std::invalid_argument("Merging blueprints failed: Component properties cannot be redefined");
          }
          else
          {
            ParameterValueType::const_iterator ownValue;
            ParameterValueType::const_iterator otherValue;
            for (ownValue = ownValues.begin(), otherValue = otherValues.begin(); ownValue != ownValues.end(); ++ownValue, ++otherValue)
            {
              if (*otherValue != *ownValue)
              {
                // No, at least one value is different. Blueprints cannot be Composed
                throw std::invalid_argument("Merging blueprints failed: Component properties cannot be redefined");
              }
            }
          }
        }
        else
        {
          // Property key doesn't exist yet, add entry to this component
          //auto ownProperties = this->GetConnection(incomingName, componentName);
          ownProperties[othersEntry.first] = othersEntry.second;
          blueprint->SetConnection(outName, inName, ownProperties);
        }
      }
    }
    else
    {
      // Create Component copying properties of other
      blueprint->SetConnection(outName, inName, newProperties);
    }
  }
}
} // namespace selx

#endif // ConfigurationReader_cxx
