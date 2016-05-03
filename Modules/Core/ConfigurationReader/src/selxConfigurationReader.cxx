#ifndef ConfigurationReader_cxx
#define ConfigurationReader_cxx

#include "selxConfigurationReader.h"
#include <ostream>

namespace selx {

// Declared outside of the class body, so it is a free function
std::ostream& operator<<(std::ostream& out, const Blueprint::ParameterValueType& vector){
  out << " : { ";
  for (auto const & value : vector)
  {
    out << value << " ";
  }
  out << " }";
  return out;
}

// Declared outside of the class body, so it is a free function
std::ostream& operator<<(std::ostream& out, const Blueprint::ParameterMapType& val){
  for (auto const & mapPair : val)
  {
    out << mapPair.first << " : { ";
    for (auto const & value : mapPair.second)
    {
      out << value << " ";
    }
    out << "}, ";
  }
  return out;
}

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

void ConfigurationReader::loadxml(const std::string &filename)
{
  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;

  read_xml(filename, pt, boost::property_tree::xml_parser::trim_whitespace);

  write_json("../parameterfile2.json", pt);

  BOOST_FOREACH(ptree::value_type &v, pt.equal_range("Component"))
  {
    //std::string componentName = v.second.data();
    std::string componentName;
    ParameterMapType componentPropertyMap;
    for (auto const & elm : v.second)
    {
      const std::string & componentKey = elm.first;
      if (componentKey == "Name")
      {
        componentName = elm.second.data();
      }

      ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
      std::string propertyKey = elm.first;
      componentPropertyMap[propertyKey] = propertyMultiValue;
    }
    std::cout << "blueprint->AddComponent(""" << componentName << """, {" << componentPropertyMap << "});" << std::endl;
  }

  BOOST_FOREACH(ptree::value_type &v, pt.equal_range("Connection"))
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
      }
      else if (connectionKey == "In")      
      {
        inName = elm.second.data();
      }
      else 
      {
        ParameterValueType propertyMultiValue = VectorizeValues(elm.second);
        std::string propertyKey = elm.first;
        componentPropertyMap[propertyKey] = propertyMultiValue;
      }

    }
    std::cout << "blueprint->AddConnection(""" << outName << """, """ << inName << """, {" << componentPropertyMap << "});" << std::endl;
    
  }


}
} // namespace selx 

#endif // ConfigurationReader_cxx
