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
project( SuperElastixSuperBuild )

if(CMAKE_CONFIGURATION_TYPES)

  list(LENGTH CMAKE_CONFIGURATION_TYPES SELX_NUMBER_OF_CONFIGURATION_TYPES)
  if ( NOT ${SELX_NUMBER_OF_CONFIGURATION_TYPES} EQUAL 1 )
    message(STATUS "CMAKE_CONFIGURATION_TYPES had ${SELX_NUMBER_OF_CONFIGURATION_TYPES} configuration types (${CMAKE_CONFIGURATION_TYPES}), but SuperElastix SuperBuild only supports exactly one configuration type at a time! It is now reset to Release.\n"
    "  Please modify CMAKE_CONFIGURATION_TYPES if you want to configure a different configuration type (Debug, MinSizeRel, or RelWithDebInfo).")
    set(CMAKE_CONFIGURATION_TYPES Release)
  endif()
  
  # Modify project name to ease distinguishing between Visual Studio solution files of different configuration types:
  project(SuperElastixSuperBuild-${CMAKE_CONFIGURATION_TYPES})

  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
  "SuperElastix SuperBuild allows CMake to generate only one configuration at a time (Debug, Release, MinSizeRel, or RelWithDebInfo)"
  FORCE)
  
  set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)
  
  message(STATUS "Selected configuration type (CMAKE_CONFIGURATION_TYPES): " ${CMAKE_CONFIGURATION_TYPES})

  if( ${CMAKE_CONFIGURATION_TYPES} STREQUAL "Debug" )
    # For a Visual C++ Debug build, use 64-bit toolset, to avoid link errors.
    set(CMAKE_GENERATOR_TOOLSET "host=x64" CACHE STRING "SuperElastix SuperBuild Visual C++ Debug uses 64-bit toolset" FORCE)
  endif()
endif()

set( SUPERELASTIXSUPERBUILD_MAJOR_VERSION 1)
set( SUPERELASTIXSUPERBUILD_MINOR_VERSION 0)
set( SUPERELASTIXSUPERBUILD_PATCH_VERSION 0)
set( SUPERELASTIXSUPERBUILD_VERSION ${SUPERELASTIXSUPERBUILD_MAJOR_VERSION}.${SUPERELASTIXSUPERBUILD_MINOR_VERSION}.${SUPERELASTIXSUPERBUILD_PATCH_VERSION})

include (CTest)

find_package( Git REQUIRED )

include( ExternalProject )

set( CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CMake
  ${CMAKE_MODULE_PATH}
)

# ---------------------------------------------------------------------
# SuperElastix SuperBuild configuration

mark_as_advanced( BUILD_SHARED_LIBS )
option( BUILD_SHARED_LIBS "Build shared libraries." OFF )

mark_as_advanced( BUILD_EXPRESS )
option( BUILD_EXPRESS "" OFF )

# Build examples by default
# Examples will be build as an external project to verify the installation of SuperElastix
option( BUILD_APPLICATIONS_EXTERNAL_PROJECT "Enable building examples." ON )

# Build tests by default
option( BUILD_TESTING "Enable building tests." ON )
if( BUILD_TESTING )
  mark_as_advanced( BUILD_INTEGRATION_TESTS )
  option( BUILD_INTEGRATION_TESTS "Also build tests that take a long time to run." OFF )
  mark_as_advanced( BUILD_LONG_UNIT_TESTS )
  option( BUILD_LONG_UNIT_TESTS "Also build tests that take a long time to run." OFF )
endif()

set(SELX_SUPERBUILD_COMMAND "") 

if (MSVC)
  set(SELX_PROGRAM_FILES_X86 "ProgramFiles(x86)") 
  find_program(SELX_INCREDIBUILD_EXE BuildConsole.exe PATHS
    "$ENV{ProgramFiles}/Xoreax/IncrediBuild"
    "$ENV{${SELX_PROGRAM_FILES_X86}}/Xoreax/IncrediBuild"
    DOC "Optional file path of IncrediBuild BuildConsole, used by the SuperBuild"
  )
endif()

function(UPDATE_SELX_SUPERBUILD_COMMAND proj)
  if (SELX_INCREDIBUILD_EXE)
    set(SELX_SUPERBUILD_COMMAND "${SELX_INCREDIBUILD_EXE}" ${proj}.sln "/build" "$(Configuration)|$(Platform)" "/UseIDEMonitor" PARENT_SCOPE) 
  endif()
endfunction()

option( SUPERELASTIX_USE_OPENMP "If available, use OpenMP to speed up components that support it." OFF )
mark_as_advanced( SUPERELASTIX_USE_OPENMP )

if(SUPERELASTIX_USE_OPENMP)
  find_package(OpenMP QUIET)
  if( OPENMP_FOUND )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  endif()
endif()

# ---------------------------------------------------------------------
# Build ITK

set( ITK_VERSION_MAJOR "4" )
set( ITK_VERSION_MINOR "13" )
set( ITK_VERSION_PATCH "0" )
set( ITK_VERSION_STRING "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}" )

mark_as_advanced( USE_SYSTEM_ITK )
option( USE_SYSTEM_ITK "Use an installed version of ITK" OFF )

if( USE_SYSTEM_ITK )
  if( ITK_DIR)
    message(STATUS "Trying to find ITK in ITK_DIR ${ITK_DIR}.")
  endif()
  find_package( ITK ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} REQUIRED )
  include( ${ITK_USE_FILE} )
else()
  include( ExternalITK )
endif()

# ---------------------------------------------------------------------
# Build Elastix

mark_as_advanced( SUPERELASTIX_USE_ELASTIX )
option( SUPERELASTIX_USE_ELASTIX "Let SuperElastix use Components based on elastix." ON )

if( SUPERELASTIX_USE_ELASTIX )
  option( USE_SYSTEM_ELASTIX "Use system install of elastix." OFF )
  mark_as_advanced( USE_SYSTEM_ELASTIX )

  if( USE_SYSTEM_ELASTIX )
    if( ELASTIX_DIR )
      message(STATUS "Trying to find elastix in ELASTIX_DIR ${ELASTIX_DIR}.")
    endif()
    find_package( Elastix REQUIRED )
    include( ${ELASTIX_USE_FILE} )
  else()
    include( ExternalElastix )
  endif()
endif()

# ---------------------------------------------------------------------
# Build Niftyreg

mark_as_advanced( SUPERELASTIX_USE_NIFTYREG )
option( SUPERELASTIX_USE_NIFTYREG "Let SuperElastix use Components based on Niftyreg." ON )

if( SUPERELASTIX_USE_NIFTYREG )
  mark_as_advanced( USE_SYSTEM_NIFTYREG )
  option( USE_SYSTEM_NIFTYREG "Use an installed version of Niftyreg." OFF )
  if( USE_SYSTEM_NIFTYREG )
    set( NIFTYREG_DIR "NOT_FOUND" CACHE PATH "Absolute path to NiftyReg install directory." )
  else()
    include( ExternalNiftyreg )
  endif()
endif()

# ---------------------------------------------------------------------
# Build Boost

mark_as_advanced( USE_SYSTEM_BOOST )
option( USE_SYSTEM_BOOST "Use an installed version of BOOST" OFF )
if( USE_SYSTEM_BOOST )
  find_package( Boost 1.65.0 EXACT REQUIRED log date_time thread regex graph program_options filesystem )
else()
  include( ExternalBoost )
endif()

# ---------------------------------------------------------------------
# Build ATen

mark_as_advanced( SUPERELASTIX_USE_ATEN )
option( SUPERELASTIX_USE_ATEN "Let SuperElastix use Components based on ATen." OFF )

if( SUPERELASTIX_USE_ATEN )
  mark_as_advanced( USE_SYSTEM_ATEN )
  option( USE_SYSTEM_ATEN "Use a pre-compiled version of ATen." OFF )

  if ( USE_SYSTEM_ATEN )
    find_package( ATen REQUIRED )
  else()
    include( ExternalATen )
  endif()
endif()

# ---------------------------------------------------------------------
# Installation

# Register the build-tree with a global CMake-registry
export( PACKAGE SuperElastixSuperBuild )

set( CMAKE_INSTALL_DIR "" )
if( WIN32 AND NOT CYGWIN )
  set( CMAKE_INSTALL_DIR CMake )
else()
  set( CMAKE_INSTALL_DIR lib/CMake/SuperElastix )
endif()

set( SUPERELASTIXSUPERBUILD_USE_FILE "${PROJECT_BINARY_DIR}/UseSuperElastixSuperBuild.cmake")
configure_file( SuperElastixSuperBuildConfig.cmake.in "${PROJECT_BINARY_DIR}/SuperElastixSuperBuildConfig.cmake" @ONLY)
configure_file( SuperElastixSuperBuildConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/SuperElastixSuperBuildConfigVersion.cmake" @ONLY)
configure_file( UseSuperElastixSuperBuild.cmake.in "${SUPERELASTIXSUPERBUILD_USE_FILE}" @ONLY )

# ---------------------------------------------------------------------
# Build SuperElastix

include( SuperElastix )

# ---------------------------------------------------------------------
# Build Applications as an external project

if( ${BUILD_APPLICATIONS_EXTERNAL_PROJECT} )
  include( ExternalApplications )
endif()




