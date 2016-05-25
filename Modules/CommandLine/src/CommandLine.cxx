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

#include "Overlord.h"
#include "selxBlueprint.h"
#include "selxConfigurationReader.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"

#include "selxElastixComponent.h"
#include "selxItkImageSink.h"

#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>

using namespace std;
using namespace selx;
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
  return os;
}




int main(int ac, char* av[])
{
  try {

    path configurationPath;
    vector<path> inputFilePaths;
    vector<path> outputFilePaths;

    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("conf", po::value<path>(&configurationPath)->required(), "Configuration file")
      ("in", po::value<vector<path>>(&inputFilePaths)->multitoken(), "Input data: images, labels, meshes, etc.")
      ("out", po::value<vector<path>>(&outputFilePaths)->multitoken(), "Output data: images, labels, meshes, etc.")
      ("graphout", po::value<path>(), "Output Graphviz dot file")
      //("levels", po::value<int>(), "number of resolution levels")
      //("iterations", po::value<vector<int>>(), "number of iterations per level")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << "\n";
      return 0;
    }

    if (vm.count("in")) {
      cout << "Number of input data: " << inputFilePaths.size() << "\n";
      int index = 0;
      for (const auto & inname : inputFilePaths)
      {
        cout << "  [" << index << "]: " << inname << "\n";
        ++index;
      }
    }
    if (vm.count("out")) {
      cout << "Number of output data: " << outputFilePaths.size() << "\n";
      int index = 0;
      for (const auto & outname : outputFilePaths)
      {
        cout << "  [" << index << "]: " << outname << "\n";
        ++index;
      }
    }
    /*
    if (vm.count("levels")) {
    cout << "number of resolution levels was set to "
    << vm["levels"].as<int>() << ".\n";
    }
    else {
    cout << "number of resolution levels was not set.\n";
    }

    if (vm.count("iterations")) {
    cout << "number of iterations per level was set to "
    << vm["iterations"].as< vector<int> >() << "\n";
    }
    else {
    cout << "number of iterations per level was not set.\n";
    }
    */

    Blueprint::Pointer blueprint;
    if (configurationPath.extension() == ".xml")
    {
      // TODO: open file here and pass a stream to the ConfigurationReader
      blueprint = ConfigurationReader::FromXML(configurationPath.string());
    }
    else if (configurationPath.extension() == ".json")
    {
      // TODO: open file here and pass a stream to the ConfigurationReader
      blueprint = ConfigurationReader::FromJson(configurationPath.string());
    }
    else {
      throw invalid_argument("Configuration file requires extension .xml or .json");
    }

    if (vm.count("graphout"))
    {
      blueprint->WriteBlueprint(vm["graphout"].as<path>().string());
    }

    ComponentFactory<DisplacementFieldItkImageFilterSinkComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceFixedComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageRegistrationMethodv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkMeanSquaresImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ElastixComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSinkComponent<2, float>>::RegisterOneFactory();

    Overlord::Pointer overlord = Overlord::New();


    vector<string> inputFileStings;
    std::transform(std::begin(inputFilePaths),
      std::end(inputFilePaths),
      std::back_inserter(inputFileStings),
      [](path p) { return p.string(); }
    );

    vector<string> outputFileStings;
    std::transform(std::begin(outputFilePaths),
      std::end(outputFilePaths),
      std::back_inserter(outputFileStings),
      [](path p) { return p.string(); }
    );


    //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
    overlord->inputFileNames = inputFileStings;
    overlord->outputFileNames = outputFileStings;

    overlord->SetBlueprint(blueprint);
    bool allUniqueComponents;
    allUniqueComponents = overlord->Configure();
    overlord->Execute();


  }
  catch (exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch (...) {
    cerr << "Exception of unknown type!\n";
  }


  return 0;
}