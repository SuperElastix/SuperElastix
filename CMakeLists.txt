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

cmake_policy( SET CMP0064 OLD )

# Explicitly add INCREMENTAL linking option to command lines.
# http://www.cmake.org/pipermail/cmake/2010-February/035174.html
#set( MSVC_INCREMENTAL_DEFAULT ON )

# ---------------------------------------------------------------------
project( SuperElastix )

enable_language(C)
enable_language(CXX)
set( CMAKE_CXX_STANDARD 11 )

# Place executables in the bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

# Place shared libraries in the lib directory
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )

# Place static libraries in the lib directory
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )

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

list( APPEND RequiredITKModules
  ITKReview
)

foreach( ITKModule ${RequiredITKModules} )
  if( NOT ${ITKModule}_LOADED )
    message( FATAL_ERROR "elastix requires that ITK is build with ${ITKModule}. Please rebuild ITK with Module_${ITKModule} set to ON." )
  endif()
endforeach()

# ---------------------------------------------------------------------
# Boost Graph Library
find_package( Boost COMPONENTS program_options filesystem system REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

# ---------------------------------------------------------------------
# Build SuperElastix

# Initialize the build system and build the core. 
# These calls are mandatory. Do not change and do
# not call initialization macros anywhere else.
include( selxModules )
include( selxApplications )

_selxmodules_initialize()
_selxapplications_initialize()

_selxmodule_enable( ModuleCore "SuperElastix" )

# User configuration
set( SUPERELASTIX_DEFAULT_MODULES
  ModuleElastix 
  ModuleExamples
  ModuleItkSmoothingRecursiveGaussianImageFilter
  ModuleItkImageRegistrationMethodv4
  ModuleItkSyNImageRegistrationMethod
  ModuleController
)

set( SUPERELASTIX_DEFAULT_APPLICATIONS
  ApplicationSuperElastixCLI
)

# TODO: Foreach() in enable_modules/applications cannot handle list of length 1
enable_application( ApplicationSuperElastixCLI )

# TODO: At the moment there is no difference between the default build 
# and the express build because ModuleCore depends on all modules.
mark_as_advanced( SUPERELASTIX_EXPRESS_BUILD )
option( SUPERELASTIX_EXPRESS_BUILD "" OFF )

if( NOT ${SUPERELASTIX_EXPRESS_BUILD} )
  enable_modules( ${SUPERELASTIX_DEFAULT_MODULES} )
  enable_applications( ${SUPERELASTIX_DEFAULT_APPLICATIONS} )
else()
  disable_modules( ${SUPERELASTIX_DEFAULT_MODULES} )
  disable_applications( ${SUPERELASTIX_DEFAULT_APPLICATIONS} )
endif()

# TODO: Functionality to disable default modules/applications

# ---------------------------------------------------------------------
# Testing

set( SUPERELASTIX_INPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/Data/Input )
set( SUPERELASTIX_OUTPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/Data/Output )
set( SUPERELASTIX_BASELINE_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/Data/Baseline )
set( SUPERELASTIX_CONFIGURATION_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Testing/Data/Configuration )

if( NOT EXISTS ${SUPERELASTIX_OUTPUT_DATA_DIR} )
  file( MAKE_DIRECTORY ${SUPERELASTIX_OUTPUT_DATA_DIR} )
endif()

if( NOT EXISTS ${SUPERELASTIX_OUTPUT_DATA_DIR} )
  message( FATAL_ERROR 
    "Could not create directory for output data. Make sure CMake has write permissions."
  )
endif()

option( SUPERELASTIX_BUILD_TESTING "Enable building tests." ON )
set( BUILD_TESTING ${SUPERELASTIX_BUILD_TESTING} )
if( ${SUPERELASTIX_BUILD_TESTING} )
  mark_as_advanced( SUPERELASTIX_BUILD_INTEGRATION_TESTS )
  option( SUPERELASTIX_BUILD_INTEGRATION_TESTS "Also build tests that take a long time to run." OFF )
  mark_as_advanced( SUPERELASTIX_BUILD_LONG_UNIT_TESTS )
  option( SUPERELASTIX_BUILD_LONG_UNIT_TESTS "Also build tests that take a long time to run." OFF )
  add_subdirectory( Testing )
endif()

# ---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( SUPERELASTIX_BUILD_DOXYGEN )
option( SUPERELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( SUPERELASTIX_BUILD_READTHEDOCS )
option( SUPERELASTIX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

# ---------------------------------------------------------------------
# Enable other projects to use SuperElastix via CMake's find_packge() and a use-file

set( SUPERELASTIX_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR} )
set( SUPERELASTIX_BINARY_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

configure_file( SuperElastixConfig.cmake.in SuperElastixConfig.cmake @ONLY IMMEDIATE )
configure_file( UseSuperElastix.cmake.in UseSuperElastix.cmake COPYONLY IMMEDIATE )
