#=========================================================================
#
#  Copyright Leiden University Medical Center, Erasmus University Medical 
#  Center and contributors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#=========================================================================

# ---------------------------------------------------------------------
# Build GoogeTest

# Download and unpack googletest at configure time
configure_file(googletest.cmake.in ${CMAKE_BINARY_DIR}/Testing/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/Testing/googletest-src
                 ${CMAKE_BINARY_DIR}/Testing/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
  include_directories("${gmock_SOURCE_DIR}/include")
endif()

set(TEST_LIBRARIES gtest_main gtest)

# ---------------------------------------------------------------------
# Download test data

# ExternalData module requires CMake version 2.8.11
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  message( FATAL_ERROR "Building tests require CMake 2.8.11 or newer." )
endif()

include( ExternalData )

list( APPEND ExternalData_URL_TEMPLATES
  "https://data.kitware.com/api/v1/file/hashsum/sha512/%(hash)/download"
)

# Scan the Data directory for content links on the form "[filename].[ext].sha512". 
# The content links contains sha512 hashes that are checked for consistensy  
# against files downloaded from servers in ExternalData_URL_TEMPLATES. These 
# files are placed in a corresponding location in the build directory.
file( GLOB_RECURSE SuperElastixDataContentLinks "*.sha512" )
foreach( SuperElastixDataContentLink ${SuperElastixDataContentLinks} )
  string( REGEX REPLACE "\\.sha512$" "" SuperElastixDataContentLink ${SuperElastixDataContentLink} )
  ExternalData_Expand_Arguments( SuperElastixData
    SuperElastixDataFileName
    DATA{${SuperElastixDataContentLink}}
  )
endforeach()

# Test data is downloaded when this target is built
ExternalData_Add_Target( SuperElastixData )

# ---------------------------------------------------------------------
# Build data manager

if( WIN32 )
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_INPUT_DATA_DIR ${SUPERELASTIX_INPUT_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_OUTPUT_DATA_DIR ${SUPERELASTIX_OUTPUT_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_BASELINE_DATA_DIR ${SUPERELASTIX_BASELINE_DATA_DIR})
  string(REGEX REPLACE "/" "\\\\\\\\" SUPERELASTIX_CONFIGURATION_DATA_DIR ${SUPERELASTIX_CONFIGURATION_DATA_DIR})
endif (WIN32)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/selxDataDirectories.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/selxDataDirectories.h
  ESCAPE_QUOTES
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
add_library( DataManager selxDataManager.cxx )


list( APPEND TEST_LIBRARIES DataManager )

# ---------------------------------------------------------------------
# Build test suite

if( ${BUILD_UNIT_TESTS} )
  add_subdirectory( Unit )
endif()

if( ${BUILD_INTEGRATION_TESTS} )
  add_subdirectory( Integration )
endif()

