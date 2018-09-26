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
#include "selxAnyFileReader.h"
#include "selxAnyFileWriter.h"
#include "selxLogger.h"
#include "selxGitInfo.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

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

namespace selx
{
  // helper function to parse command line arguments for log level
  std::istream& operator>>(std::istream& in, selx::LogLevel& loglevel)
  {
    std::string token;
    in >> token;
    if (token == "off")
      loglevel = selx::LogLevel::OFF;
    else if (token == "critical")
      loglevel = selx::LogLevel::CRT;
    else if (token == "error")
      loglevel = selx::LogLevel::ERR;
    else if (token == "warning")
      loglevel = selx::LogLevel::WRN;
    else if (token == "info")
      loglevel = selx::LogLevel::INF;
    else if (token == "debug")
      loglevel = selx::LogLevel::DBG;
    else if (token == "trace")
      loglevel = selx::LogLevel::TRC;
    else
      in.setstate(std::ios_base::failbit);
    return in;
  }
}

int
main( int ac, char * av[] )
{

  selx::Logger::Pointer logger = selx::Logger::New();
  
  typedef std::vector< std::string > VectorOfStringsType;
  typedef std::vector< boost::filesystem::path > VectorOfPathsType;

  boost::filesystem::path logPath;
  // default log level
  selx::LogLevel logLevel = selx::LogLevel::WRN;

  boost::filesystem::path            configurationPath;
  VectorOfPathsType                   configurationPaths;

  VectorOfStringsType inputPairs;
  VectorOfStringsType outputPairs;

  boost::program_options::variables_map vm;

  try
  {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
      ( "help", "produce help message" )
      ( "revision-sha", "produce git revision SHA-1 hash of SuperElastix source" )
      ("conf", boost::program_options::value< VectorOfPathsType >(&configurationPaths)->required()->multitoken(), "Configuration file: single or multiple Blueprints [.xml|.json]")
      ("in", boost::program_options::value< VectorOfStringsType >(&inputPairs)->multitoken(), "Input data: images, labels, meshes, etc. Usage arg: <name>=<path> (or multiple pairs)")
      ("out", boost::program_options::value< VectorOfStringsType >(&outputPairs)->multitoken(), "Output data: images, labels, meshes, etc. Usage arg: <name>=<path> (or multiple pairs)")
      ("graphout", boost::program_options::value< boost::filesystem::path >(), "Output Graphviz dot file")
      ("logfile", boost::program_options::value< boost::filesystem::path >(&logPath), "Log output file")
      ("loglevel", boost::program_options::value< selx::LogLevel >(&logLevel), "Log level [off|critical|error|warning|info|debug|trace]")
      ;

    boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

    if( vm.count( "help" ) )
    {
      std::cout
        << "SuperElastix GIT Revision SHA-1: " << selx::GitInfo::GetRevisionSha() << '\n'
        << desc << std::endl;
      return 0;
    }
    if (vm.count("revision-sha"))
    {
      std::cout << selx::GitInfo::GetRevisionSha() << std::endl;
      return 0;
    }
    boost::program_options::notify(vm);
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    std::cerr << "See 'SuperElastix --help' for help" << "\n";
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unknown error!" << "\n";
    return 1;
  }

  try 
  {
    // optionally, stream to log file
    std::ofstream outfile;
    if ( vm.count("logfile") )
    {
      outfile =  std::ofstream(logPath.c_str());
      logger->AddStream("logfile", outfile);
    }

    logger->AddStream("cout", std::cout);
    logger->SetLogLevel(logLevel);
   
    // instantiate a SuperElastixFilter that is loaded with default components
    selx::SuperElastixFilter::Pointer superElastixFilter = selx::SuperElastixFilter::New();

    superElastixFilter->SetLogger(logger);

    // create empty blueprint
    selx::Blueprint::Pointer blueprint = selx::Blueprint::New();
    blueprint->SetLogger(logger);
    for (const auto & configurationPath : configurationPaths)
    {
      blueprint->MergeFromFile(configurationPath.string());
    }

    if( vm.count( "graphout" ) )
    {
      blueprint->Write(vm["graphout"].as< boost::filesystem::path >().string());
    }

    // The Blueprint needs to be set to superElastixFilter before GetInputFileReader and GetOutputFileWriter should be called.
    superElastixFilter->SetBlueprint(blueprint);

    // Store the readers so that they will not be destroyed before the pipeline is executed.
    std::vector< selx::AnyFileReader::Pointer > fileReaders;
    // Store the writers for the update call
    std::vector< selx::AnyFileWriter::Pointer > fileWriters;

    if( vm.count( "in" ) )
    {
      logger->Log( selx::LogLevel::INF, "Preparing input data ... ");
      for( const auto & inputPair : inputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, inputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];

        // since we do not know which reader type we should instantiate for input "name",
        // we ask SuperElastix for a reader that matches the type of the source component "name"
        logger->Log( selx::LogLevel::INF, "Preparing input '" + name + "': " + path + " ..." );
        selx::AnyFileReader::Pointer reader = superElastixFilter->GetInputFileReader( name );
        reader->SetFileName( path );
        superElastixFilter->SetInput( name, reader->GetOutput() );
        fileReaders.push_back( reader );
        logger->Log( selx::LogLevel::INF, "Preparing input '" + name + "': " + path + " ... Done" );
      }
      logger->Log( selx::LogLevel::INF, "Preparing input data ... Done");
    }
    else
    {
      logger->Log( selx::LogLevel::INF, "No input data specified.");
    }

    if( vm.count( "out" ) )
    {
      logger->Log( selx::LogLevel::INF, "Preparing output data ... ");
      for( const auto & outputPair : outputPairs )
      {
        VectorOfStringsType nameAndPath;
        boost::split( nameAndPath, outputPair, boost::is_any_of( "=" ) );  // NameAndPath == { "name","path" }
        const std::string & name = nameAndPath[ 0 ];
        const std::string & path = nameAndPath[ 1 ];

        // since we do not know which writer type we should instantiate for output "name",
        // we ask SuperElastix for a writer that matches the type of the sink component "name"
        logger->Log( selx::LogLevel::INF, "Preparing output '" + name + "': " + path + " ..." );
        selx::AnyFileWriter::Pointer writer = superElastixFilter->GetOutputFileWriter( name );
        writer->SetFileName( path );
        writer->SetInput( superElastixFilter->GetOutput( name ) );
        fileWriters.push_back( writer );
        logger->Log( selx::LogLevel::INF, "Preparing output '" + name + "': " + path + " ... Done" );
      }
    }
    else
    {
      logger->Log( selx::LogLevel::INF, "No output data specified.");
    }

    /* Execute SuperElastix by updating the writers */
    logger->Log( selx::LogLevel::INF, "Executing ...");
    for( auto & writer : fileWriters )
    {
      writer->Update();
    }
    logger->Log(selx:: LogLevel::INF, "Executing ... Done");
  }
  catch( std::exception & e )
  {
    logger->Log( selx::LogLevel::CRT, "Executing ... Error");
    logger->Log( selx::LogLevel::CRT, e.what());
    std::cerr << e.what();
    return 1;
  }
  catch( ... )
  {
    logger->Log( selx::LogLevel::CRT, "Executing ... Error");
    logger->Log( selx::LogLevel::CRT, "Exception of unknown type!");
    std::cerr << "Exception of unknown type!";
    return 1;
  }

  return 0;
}
