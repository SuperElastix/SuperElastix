#ifndef ConfigurationReader_cxx
#define ConfigurationReader_cxx

#include "selxConfigurationReader.h"
#include <ostream>

namespace selx {

 
ConfigurationReader::ParameterValueType ConfigurationReader::VectorizeValues(ComponentOrConnectionTreeType componentOrConnectionTree)
{
  std::string propertySingleValue = componentOrConnectionTree.data();
  ParameterValueType propertyMultiValue;
  for (auto const & value : componentOrConnectionTree)
  {
    propertyMultiValue.push_back(value.second.data());
  }
  if (propertyMultiValue.size() > 0)
  {
    if (propertySingleValue != "")
    {
      throw std::invalid_argument("XML tree should have either 1 unnamed element or multiple named properties");
    }
  }
  else
  {
    propertyMultiValue.push_back(propertySingleValue);
  }
  return propertyMultiValue;
}

ConfigurationReader::BlueprintPointerType ConfigurationReader::FromXML(const std::string &filename)
{
  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;
 
  read_xml(filename, pt, boost::property_tree::xml_parser::trim_whitespace);

  return FromPropertyTree(pt);
}

ConfigurationReader::BlueprintPointerType ConfigurationReader::FromJson(const std::string &filename)
{
  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;

  read_json(filename, pt);
  return FromPropertyTree(pt);
}

ConfigurationReader::BlueprintPointerType ConfigurationReader::FromPropertyTree(const PropertyTreeType &pt)
{

  BlueprintPointerType blueprint = Blueprint::New();

  BOOST_FOREACH(const PropertyTreeType::value_type &v, pt.equal_range("Component"))
  {
    std::string componentName;
    ParameterMapType componentPropertyMap;
    for (auto const & elm : v.second)
    {
      const std::string & componentKey = elm.first;
      if (componentKey == "Name")
      {
        componentName = elm.second.data();
        continue;
      }

      ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
      std::string propertyKey = elm.first;
      componentPropertyMap[propertyKey] = propertyMultiValue;
    }

    blueprint->AddComponent(componentName, componentPropertyMap);
  }

  BOOST_FOREACH(const PropertyTreeType::value_type &v, pt.equal_range("Connection"))
  {
    std::string connectionName = v.second.data();
    if (connectionName != "")
    {
      std::cout << "warning connectionName """ << connectionName << """ is ignored" << std::endl;
    }
    std::string outName;
    std::string inName;
    ParameterMapType componentPropertyMap;

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
        std::cout << "Warning: connection 'Name'-s are ignored." << std::endl;
        continue;
      }
      else
      {
        ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
        std::string propertyKey = elm.first;
        componentPropertyMap[propertyKey] = propertyMultiValue;
      }

    }

    blueprint->AddConnection(outName, inName, componentPropertyMap);

  }
  return blueprint;

}

} // namespace selx 

#endif // ConfigurationReader_cxx
