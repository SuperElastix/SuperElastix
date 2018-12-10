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

cmake_minimum_required( VERSION 3.5.1 )

# ---------------------------------------------------------------------
project( SuperElastix )

set( SUPERELASTIX_MAJOR_VERSION 1)
set( SUPERELASTIX_MINOR_VERSION 0)
set( SUPERELASTIX_PATCH_VERSION 0)
set( SUPERELASTIX_VERSION ${SUPERELASTIX_MAJOR_VERSION}.${SUPERELASTIX_MINOR_VERSION}.${SUPERELASTIX_PATCH_VERSION})

# Place executables in the bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )

# Place shared libraries in the lib directory
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )

# Place static libraries in the lib directory
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )

# Include SuperElastix CMake scripts
list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" )

# -----------------------------------------------------------------
# Compilation settings

enable_language( CXX )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED on )

mark_as_advanced( SUPERELASTIX_BUILD_SHARED_LIBS )
option( SUPERELASTIX_BUILD_SHARED_LIBS "Build SuperElastix with shared libraries." OFF )
set( BUILD_SHARED_LIBS ${SUPERELASTIX_BUILD_SHARED_LIBS} )

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
  
  # Avoid many warnings, originating mainly from NiftyReg
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  # Avoid many warnings, originating mainly from GoogleTest
  add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  

  if( ${CMAKE_CONFIGURATION_TYPES} STREQUAL "Debug" )
    # For a Visual C++ Debug build, use 64-bit toolset, to avoid link errors.
    set(CMAKE_GENERATOR_TOOLSET "host=x64" CACHE STRING "SuperElastix Visual C++ Debug uses 64-bit toolset" FORCE)
  endif()

  # Explicitly add INCREMENTAL linking option to command lines.
  # http://www.cmake.org/pipermail/cmake/2010-February/035174.html
  # set( MSVC_INCREMENTAL_DEFAULT ON )
endif()

if( WIN32 )
  # If building on Windows: The target machine should be Windows 7 or higher.
  # Note that the Windows version should match with BOOST_USE_WINAPI_VERSION. 
  add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# ---------------------------------------------------------------------
# Use SuperBuild. 

# The SuperBuild is typically used to build dependencies. Should
# you wish to build them yourself (e.g. if you need a specific
# version of ITK), set USE_SUPERBUILD to OFF _and_ USE_SYSTEM_ITK to ON, 
# USE_SYSTEM_BOOST to ON, USE_SYSTEM_GTEST to ON or USE_SYSTEM_ELASTIX
# to ON. The find_package() command will then load your system libraries
# instead.

option( USE_SUPERBUILD "Use dependencies built by SuperBuild." ON )
mark_as_advanced( USE_SUPERBUILD )

if( USE_SUPERBUILD )
  find_package( SuperElastixSuperBuild REQUIRED )
  include( ${SUPERELASTIXSUPERBUILD_USE_FILE} )
endif()

# ---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

# ---------------------------------------------------------------------
# Boost Library

set( Boost_USE_STATIC_LIBS ON ) # only find static libs
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost 1.65.0 EXACT COMPONENTS program_options filesystem system regex REQUIRED QUIET ) # graph ?
include_directories( ${Boost_INCLUDE_DIRS} )

# ---------------------------------------------------------------------
# SuperElastix configuration 

option( BUILD_APPLICATIONS "Build applications." ON )

mark_as_advanced( BUILD_SHARED_LIBS )
option( BUILD_SHARED_LIBS "Build shared libraries." OFF )

# ---------------------------------------------------------------------
# SuperElastix Build

# Initialize the build system and build the core. These calls are mandatory. 
# Do not change and do not call anywhere else.
message( STATUS "Enabling modules ..." )
include( selxModules )
_selxmodules_initialize()
enable_modules()
message( STATUS "Enabling modules ... Done" )

# Originally based on the answer by Naszta at
# https://stackoverflow.com/questions/6526451/how-to-include-git-commit-number-into-a-c-executable
find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE SELX_GIT_REVISION_SHA
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  else()
    set(SELX_GIT_REVISION_SHA 0)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Modules/Core/selxGitRevisionSha.h.in ${CMAKE_CURRENT_BINARY_DIR}/ModuleCore/selxGitRevisionSha.h @ONLY)

include( ${ModuleFilter_SOURCE_DIR}/CompiledLibraryComponents.cmake )
mark_as_advanced( COMPILED_LIBRARY_CONFIG_DIR )
set( COMPILED_LIBRARY_CONFIG_DIR ${PROJECT_BINARY_DIR} CACHE PATH "Path where a custom selxCompiledLibraryComponents.h can be found. Defaults to automatic generated file in ${PROJECT_BINARY_DIR}")
include_directories(${COMPILED_LIBRARY_CONFIG_DIR})
list( APPEND SUPERELASTIX_INCLUDE_DIRS ${COMPILED_LIBRARY_CONFIG_DIR} )
    
option( BUILD_UNIT_TESTS "Also build tests that take a long time to run." ON )


# Build applications
if( ${BUILD_APPLICATIONS} )
  message( STATUS "Enabling applications ..." )
  include( selxApplications )
  _selxapplications_initialize()
  enable_applications( SUPERELASTIX_APPLICATIONS )
  message( STATUS "Enabling applications ... Done" )
endif()

# TODO: Functionality to disable default modules/applications

# ---------------------------------------------------------------------
# Testing

set( SUPERELASTIX_INPUT_DATA_DIR ${PROJECT_BINARY_DIR}/Testing/Data/Input )
set( SUPERELASTIX_OUTPUT_DATA_DIR ${PROJECT_BINARY_DIR}/Testing/Data/Output )
set( SUPERELASTIX_BASELINE_DATA_DIR ${PROJECT_BINARY_DIR}/Testing/Data/Baseline )
set( SUPERELASTIX_CONFIGURATION_DATA_DIR ${PROJECT_SOURCE_DIR}/Testing/Data/Configuration )

if( NOT EXISTS ${SUPERELASTIX_OUTPUT_DATA_DIR} )
  file( MAKE_DIRECTORY ${SUPERELASTIX_OUTPUT_DATA_DIR} )
endif()

if( NOT EXISTS ${SUPERELASTIX_OUTPUT_DATA_DIR} )
  message( FATAL_ERROR "Could not create directory for output data. Make sure CMake has write permissions." )
endif()

include( CTest )
option( BUILD_TESTING "Build SuperElastix unit tests." ON )

if( ${BUILD_TESTING} )
  mark_as_advanced( BUILD_UNIT_TESTS )
  option( BUILD_UNIT_TESTS "Also build tests that take a long time to run." ON )
  mark_as_advanced( BUILD_LONG_UNIT_TESTS )
  option( BUILD_LONG_UNIT_TESTS "Also build tests that take a long time to run." OFF )
  mark_as_advanced( BUILD_INTEGRATION_TESTS )
  option( BUILD_INTEGRATION_TESTS "Build integration tests for applications." OFF )
  add_subdirectory( Testing )
endif()

# ---------------------------------------------------------------------
# Documentation

option( BUILD_DOXYGEN "Build Doxygen documentation." OFF )
mark_as_advanced( BUILD_DOXYGEN )

option( BUILD_READTHEDOCS "Build readthedocs.org documentation." OFF )
mark_as_advanced( BUILD_READTHEDOCS )

# ---------------------------------------------------------------------
# Installation

set( CMAKE_INSTALL_DIR "" )
if( WIN32 AND NOT CYGWIN )
  set( CMAKE_INSTALL_DIR CMake )
else()
  set( CMAKE_INSTALL_DIR lib/CMake/SuperElastix )
endif()
 
# Register the build-tree with a global CMake-registry
export( PACKAGE SuperElastix )
 
# Create the SuperElastix.cmake files
# ... for the build tree
set( SUPERELASTIX_INSTALL_INCLUDE_DIRS ${SUPERELASTIX_INCLUDE_DIRS} )
set( SUPERELASTIX_INSTALL_LIBRARY_DIRS ${SUPERELASTIX_LIBRARY_DIRS} )
set( SUPERELASTIX_INSTALL_LIBRARIES ${SUPERELASTIX_LIBRARIES} )
set( SUPERELASTIX_INSTALL_USE_FILE "${PROJECT_BINARY_DIR}/UseSuperElastix.cmake" )
set( SUPERELASTIX_TARGETS_FILE "${CMAKE_BINARY_DIR}/SuperElastixTargets.cmake" )
configure_file( SuperElastixConfig.cmake.in "${PROJECT_BINARY_DIR}/SuperElastixConfig.cmake" @ONLY)
configure_file( SuperElastixConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/SuperElastixConfigVersion.cmake" @ONLY)
configure_file( UseSuperElastix.cmake.in "${SUPERELASTIX_INSTALL_USE_FILE}" @ONLY )

# ... for the install tree
set( SUPERELASTIX_INSTALL_INCLUDE_DIRS include )
set( SUPERELASTIX_INSTALL_LIBRARY_DIRS lib )
set( SUPERELASTIX_INSTALL_LIBRARIES ModuleFilter ModuleBlueprints ModuleLogger )
set( SUPERELASTIX_INSTALL_USE_FILE ${CMAKE_INSTALL_DIR}/UseSuperElastix.cmake )
configure_file( SuperElastixConfig.cmake.in install/SuperElastixConfig.cmake @ONLY)
configure_file( SuperElastixConfigVersion.cmake.in install/SuperElastixConfigVersion.cmake @ONLY)
configure_file( UseSuperElastix.cmake.in install/UseSuperElastix.cmake @ONLY ) 
configure_file(CMake/CTestCustom.cmake.in CTestCustom.cmake @ONLY)

install( FILES 
         ${PROJECT_BINARY_DIR}/install/SuperElastixConfig.cmake
         ${PROJECT_BINARY_DIR}/install/SuperElastixConfigVersion.cmake
         ${PROJECT_BINARY_DIR}/install/UseSuperElastix.cmake
         DESTINATION ${CMAKE_INSTALL_DIR} )

install( FILES 
         Modules/Filter/include/selxSuperElastixFilter.h
         Modules/Filter/include/selxSuperElastixFilterCustomComponents.h
         Modules/Filter/include/itkUniquePointerDataObjectDecorator.h
         Modules/Filter/include/itkUniquePointerDataObjectDecorator.hxx
  Modules/Blueprints/include/Blueprint.h
  Modules/Logger/include/Logger.h
         Modules/FileIO/include/selxAnyFileReader.h
         Modules/FileIO/include/selxAnyFileWriter.h
         DESTINATION include )

install( TARGETS ModuleFilter ModuleBlueprints ModuleLogger ModuleCore
         DESTINATION lib )
