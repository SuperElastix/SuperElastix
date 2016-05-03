#ifndef ConfigurationReader_h
#define ConfigurationReader_h

#include "selxBlueprint.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <map>

namespace selx {

class ConfigurationReader
{
public:

  typedef Blueprint::ParameterKeyType                                ParameterKeyType;
  typedef Blueprint::ParameterValueType                              ParameterValueType;
  typedef Blueprint::ParameterMapType                                ParameterMapType;
  typedef Blueprint::Pointer                                         BlueprintPointerType;

  static BlueprintPointerType FromXML(const std::string &filename);
  static BlueprintPointerType FromJson(const std::string &filename);
  
private:

  typedef const boost::property_tree::ptree::basic_ptree& ComponentOrConnectionTreeType;

  static ParameterValueType VectorizeValues(ComponentOrConnectionTreeType componentOrConnectionTree);

};

}

#endif // #define ConfigurationReader_h