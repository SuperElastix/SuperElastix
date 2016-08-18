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

cmake_minimum_required( VERSION 3.0.2 )

# Explicitly add INCREMENTAL linking option to command lines.
# http://www.cmake.org/pipermail/cmake/2010-February/035174.html
#set( MSVC_INCREMENTAL_DEFAULT ON )

# ---------------------------------------------------------------------
project( SuperElastix )

enable_language(C)
enable_language(CXX)
set( CMAKE_CXX_STANDARD 11 )

# Place libraries and executables in the bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/bin"
)

# Include SuperElastix CMake scripts
list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" )

# -----------------------------------------------------------------
# Compiler-dependent settings

# GCC
if( ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU )
  add_definitions(
    -DVCL_CAN_STATIC_CONST_INIT_FLOAT=0
    -DVCL_CAN_STATIC_CONST_INIT_INT=0
  )
endif()

# MSVC
if( ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC )
  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "Source code directory path length is too long for MSVC (${n} > 50)."
      "Please move the source code directory to a directory with a shorter path."
    )
  endif()

  string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "Build directory path length is too long for MSVC (${n} > 50)."
      "Please move the build directory to a directory with a shorter path."
    )
  endif()
endif()

# ---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxRequiredITKModules.cmake")

# ---------------------------------------------------------------------
# Boost Graph Library
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

# ---------------------------------------------------------------------
# Build SuperElastix

# For now we just enable all modules
include( selxModules )
selxmodule_enable( ModuleCore )
selxmodule_enable( ModuleExamples )
selxmodule_enable( ModuleSinksAndSources )
selxmodule_enable( ModuleItkSmoothingRecursiveGaussianImageFilter )
selxmodule_enable( ModuleItkImageRegistrationMethodv4 )
selxmodule_enable( ModuleItkSyNImageRegistrationMethod )
selxmodule_enable( ModuleElastix )
selxmodule_enable( ModuleController )
#selxmodule_enable( ModuleCommandLine )
include(${CMAKE_SOURCE_DIR}/Modules/CommandLine/CommandLine.cmake)
# TODO: Build tests depending on enabled modules

# ---------------------------------------------------------------------
# Testing

option( SUPERELASTIX_BUILD_TESTING "Enable building tests." ON )
if( ${SUPERELASTIX_BUILD_TESTING} )
  mark_as_advanced( SUPERELASTIX_BUILD_LONG_TESTS )
  option( SUPERELASTIX_BUILD_LONG_TESTS "Also build tests that take a long time to run." OFF )
  enable_testing()
  add_subdirectory( Testing )
endif()

# ---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( SUPERELASTIX_BUILD_DOXYGEN )
option( SUPERELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( SUPERELASTIX_BUILD_READTHEDOCS )
option( SUPERELASTIX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )


# ---------------------------------------------------------------------
# Install
# When building the "INSTALL" project of SuperElastix these files are installed

# Copy the SuperElastix executable
install(TARGETS "SuperElastix"
  RUNTIME DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/installed"
)

# Copy some of the configuration files
install(FILES "${SUPERELASTIX_UNITTEST_CONFIGURATION_DATA_DIR}/itkv4_SVF_ANTsCC.json" 
  "${SUPERELASTIX_UNITTEST_CONFIGURATION_DATA_DIR}/itkv4_SVF_MSD.json"
  "${SUPERELASTIX_UNITTEST_CONFIGURATION_DATA_DIR}/elastix_Bspline_NC.json"
  "${SUPERELASTIX_UNITTEST_CONFIGURATION_DATA_DIR}/elastix_Bspline_MSD.json"
  DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/installed/Configuration"
)

# Copy some of the input data files
#TODO: data files are only present if SUPERELASTIX_BUILD_TESTING is enabled
install(FILES "${SUPERELASTIX_UNITTEST_INPUT_DATA_DIR}/coneA2d64.mhd"
  "${SUPERELASTIX_UNITTEST_INPUT_DATA_DIR}/coneA2d64.raw"
  "${SUPERELASTIX_UNITTEST_INPUT_DATA_DIR}/coneB2d64.mhd"
  "${SUPERELASTIX_UNITTEST_INPUT_DATA_DIR}/coneB2d64.raw"
  DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/installed/Data/"
)

# Copy the demo scripts
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Install/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/installed" FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_EXECUTE GROUP_READ GROUP_WRITE )
