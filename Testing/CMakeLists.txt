#---------------------------------------------------------------------
# Setup GoogleTest

find_package( Git )
if( NOT EXISTS "${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git" AND GIT_EXECUTABLE )
  execute_process( 
  	COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_QUIET
  )
endif()

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git )
  message( 
  	FATAL_ERROR "Could not find GoogleTest submodule. Please run 
  	git submodule update --init in the source directory to compile tests."
  )
endif()

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
  include( ${CMAKE_SOURCE_DIR}/CMake/elxGoogleTestWinConfig.cmake )
endif()

add_subdirectory( GoogleTest )

include_directories(
  ${gtest_SOURCE_DIR}/include
)

set( TEST_LIBRARIES
  gtest_main
)

#---------------------------------------------------------------------
# Download unit test and benchmark data

# ExternalData module was introduced in CMake version 2.8.11
cmake_minimum_required( VERSION 2.8.11 )

include( ExternalData )

list( APPEND ExternalData_URL_TEMPLATES
  "http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
  "https://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
)

# Scan the Data directory for content links on the form "[filename].[ext].md5". 
# The content links contains md5 hashes that are checked for consistensy  
# against corresponding files downloaded from servers in 
# ExternalData_URL_TEMPLATES. These files are placed in a corresponding  
# location in the build directory.
file( GLOB_RECURSE ElastixDataContentLinks "*.md5" )
foreach(ElastixDataContentLink ${ElastixDataContentLinks})
  string( REGEX REPLACE "\\.md5$" "" ElastixDataContentLink ${ElastixDataContentLink} )
  ExternalData_Expand_Arguments( ElastixData
    ElastixDataFilenames
    DATA{${ElastixDataContentLink}}
  )
endforeach()

# Test data is downloaded when this target is built
ExternalData_Add_Target( ElastixData )

#---------------------------------------------------------------------
# Build tests
add_subdirectory( Unit )

# Build benchmarks
if( ${ELASTIX_BUILD_BENCHMARKING} )
	add_subdirectory( Benchmark )
endif()
