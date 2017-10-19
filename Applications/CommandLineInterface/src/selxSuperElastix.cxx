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

#include "selxSuperElastixFilter.h"
#include "selxBlueprintImpl.h"
#include "selxConfigurationReader.h"
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"
#include "selxLogger.h"

#include <boost/algorithm/string.hpp>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>

template< class T >
std::ostream &
operator<<( std::ostream & os, const std::vector< T > & v )
{
  std::copy( v.begin(), v.end(), std::ostream_iterator< T >( os, " " ) );
  return os;
}


int
main( int ac, char * av[] )
{
  try
  {
    typedef std::vector< std::string > VectorOfStringsType;
    typedef std::vector< boost::filesystem::path > VectorOfPathsType;

    // instantiate a SuperElastixFilter that is loaded with default components
    selx::SuperElastixFilter::Pointer superElastixFilter = selx::SuperElastixFilter::New();

    // add logger
    selx::Logger::Pointer logger = selx::Logger::New();
    superElastixFilter->SetLogger(logger);

    boost::filesystem::path            configurationPath;
    VectorOfPathsType                   configurationPaths;

    VectorOfStringsType inputPairs;
    VectorOfStringsType outputPairs;

    // Store the reader so that they will not be destroyed before the pipeline is executed.
    std::vector< selx::AnyFileReader::Pointer > fileReaders;
    // Store the writers for the update call
    //vector<ImageWriter2DType::Pointer> fileWriters;
    std::vector< selx::AnyFileWriter::Pointer > fileWriters;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
      ( "help", "produce help message" )
      ("conf", boost::program_options::value< VectorOfPathsType >(&configurationPaths)->required()->multitoken(), "Configuration file")
      ("in", boost::program_options::value< VectorOfStringsType >(&inputPairs)->multitoken(), "Input data: images, labels, meshes, etc. Usage <name>=<path>")
      ( "out", po::value< VectorOfStringsType >( &outputPairs )->multitoken(), "Output data: images, labels, meshes, etc. Usage <name>=<path>" )
      ( "graphout", po::value< fs::path >(), "Output Graphviz dot file" )
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( ac, av, desc ), vm );
    po::notify( vm );

    if( vm.count( "help" ) )
    {
      std::cout << desc << "\n";
      return 0;
    }

    // create empty blueprint
    typedef  std::shared_ptr< selx::BlueprintImpl> BlueprintImplPointer;
    BlueprintImplPointer blueprintImpl = BlueprintImplPointer(new selx::BlueprintImpl());
    for (const auto & configurationPath : configurationPaths)
    {
      selx::ConfigurationReader::MergeFromFile(blueprintImpl, configurationPath);
    }

    if( vm.count( "graphout" ) )
    {
      blueprintImpl->Write( vm[ "graphout" ].as< fs::path >().string() );
    }

    //turn the blueprint into an itkObject to connect to the superElastix itkFilter
    selx::Blueprint::Pointer blueprint = selx::Blueprint::New();
    blueprint->SetBlueprint(*blueprintImpl);
    superElastixFilter->SetBlueprint(blueprint);

    if( vm.count( "in" ) )
    {
      logger->Log( LogLevel::INF, "Preparing input data ... ");
      int index = 0;
      for( const auto & inputPair : inputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, inputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];



        // since we do not know which reader type we should instantiate for input "name",
        // we ask SuperElastix for a reader that matches the type of the source component "name"
        logger->Log( LogLevel::INF, "Preparing input {0} ...", name );
        selx::AnyFileReader::Pointer reader = superElastixFilter->GetInputFileReader( name );
        reader->SetFileName( path );
        superElastixFilter->SetInput( name, reader->GetOutput() );
        fileReaders.push_back( reader );

        logger->Log( LogLevel::INF, "Preparing input {0} ... Done", name );
        std::cout << "Input data " << index << " " << name << " : " << path << "\n";
        ++index;
      }
      logger->Log( LogLevel::INF, "Preparing input data ... Done");
    }
    else
    {
      logger->Log( LogLevel::INF, "No input data specified.");
    }

    if( vm.count( "out" ) )
    {
      logger->Log( LogLevel::INF, "Preparing output data ... ");
      int index = 0;
      for( const auto & outputPair : outputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, outputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];

        // since we do not know which writer type we should instantiate for output "name",
        // we ask SuperElastix for a writer that matches the type of the sink component "name"
        logger->Log( LogLevel::INF, "Preparing output {0} ...", name );
        selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( name );
        //ImageWriter2DType::Pointer writer = ImageWriter2DType::New();
        writer->SetFileName( path );
        //writer->SetInput(superElastixFilter->GetOutput<Image2DType>(name));
        writer->SetInput( superElastixFilter->GetOutput( name ) );
        fileWriters.push_back( writer );
        logger->Log( LogLevel::INF, "Preparing output {0} ... Done", name );
        ++index;

      }
    }
    else
    {
      logger->Log( LogLevel::INF, "No output data specified.");
    }

    /* Execute SuperElastix by updating the writers */
    logger->Log( LogLevel::INF, "Executing ...");
    for( auto & writer : fileWriters )
    {
      writer->Update();
    }
    logger->Log( LogLevel::INF, "Executing ... Done");
  }
  catch( std::exception & e )
  {
    logger->Log( LogLevel::ERR, "Executing ... Error");
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch( ... )
  {
    logger->Log( LogLevel::ERR, "Executing ... Error");
    std::cerr << "Exception of unknown type!\n";
  }

  return 0;
}
