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

# ---------------------------------------------------------------------
project( SuperElastixSuperBuild )

find_package( Git REQUIRED )

include( ExternalProject )

set( CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CMake
  ${CMAKE_MODULE_PATH}
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "SuperBuild install directory" FORCE)

# ---------------------------------------------------------------------
# SuperElastix SuperBuild configuration

# Build release by default
if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

# Build examples by default
# Examples will be build as an external project to verify the installation of SuperElastix
option( SUPERELASTIX_BUILD_EXAMPLES "Enable building examples." ON )

# Build tests by default
option( SUPERELASTIX_BUILD_TESTING "Enable building tests." ON )
if( SUPERELASTIX_BUILD_TESTING )
  option( SUPERELASTIX_BUILD_BENCHMARKING "Enable building benchmarks." ON )
  option( SUPERELASTIX_BUILD_DASHBOARD "Enable building benchmarks." ON )
endif()

# Do not build dashboard by default
option( SUPERELASTIX_BUILD_DASHBOARD "Enable building dashboard." OFF )

# ---------------------------------------------------------------------
# Build ITK

set( ITK_VERSION_MAJOR "4" )
set( ITK_VERSION_MINOR "8" )
set( ITK_VERSION_PATCH "1" )
set( ITK_VERSION_STRING "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}" )

mark_as_advanced( USE_SYSTEM_ITK )
option( USE_SYSTEM_ITK "Use an installed version of ITK" OFF )

if( USE_SYSTEM_ITK )
  find_package( ITK ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} REQUIRED )
  include( elxRequiredITKModules )
else()
  include( ExternalITK )
endif()

# ---------------------------------------------------------------------
# Build Old Elastix

mark_as_advanced( SUPERELASTIX_BUILD_ELASTIX )
option( SUPERELASTIX_BUILD_ELASTIX "Build support for Elastix ${ELASTIX_VERSION_STRING}." ON )
if( SUPERELASTIX_BUILD_ELASTIX )
  mark_as_advanced( USE_SYSTEM_ELASTIX )
  option( USE_SYSTEM_ELASTIX "Use an installed version of Elastix." OFF )
  if( USE_SYSTEM_ELASTIX )
    if( NOT EXISTS ${ELASTIX_USE_FILE} )
      set( ELASTIX_USE_FILE "NOT_FOUND" CACHE PATH "Absolute path to UseElastix.cmake in the root of the elastix build tree." )
      message( FATAL_ERROR "Could not find the ELASTIX_USE_FILE (${ELASTIX_USE_FILE}). Use the SuperBuild or manually point the ELASTIX_USE_FILE CMake variable to the UseElastix.cmake file in the root of your elastix build tree." )
    endif()
  else()
    include( ExternalElastix )
  endif()
endif()

# ---------------------------------------------------------------------
# Boost Graph Library

mark_as_advanced( USE_SYSTEM_BOOST )
option( USE_SYSTEM_BOOST "Use an installed version of BOOST" OFF )
if( USE_SYSTEM_BOOST )
  find_package( BOOST REQUIRED graph program_options filesystem)
else()
  include( ExternalBoost )
endif()

# ---------------------------------------------------------------------
# Build GoogleTest

mark_as_advanced( USE_SYSTEM_GOOGLETEST )
option( USE_SYSTEM_GOOGLETEST "Use a pre-compiled version of GoogleTest. " OFF ) 
mark_as_advanced( USE_SYSTEM_GOOGLETEST ) 
if( SUPERELASTIX_BUILD_TESTING )
  mark_as_advanced( SUPERELASTIX_BUILD_LONG_TESTS )
  option( SUPERELASTIX_BUILD_LONG_TESTS "Also build tests that take a long time to run." OFF )
  if ( USE_SYSTEM_GOOGLETEST ) 
    find_package( GTest REQUIRED ) 
  else() 
    include( ExternalGoogleTest ) 
  endif() 
endif() 

# ---------------------------------------------------------------------
# Build SuperElastix

include( SuperElastix )

# ---------------------------------------------------------------------
# Build Applications as an external project as an integration test

include( ExternalApplications )



