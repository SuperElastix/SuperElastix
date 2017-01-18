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

set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "SuperBuild install directory" FORC )

# ---------------------------------------------------------------------
# SuperElastix SuperBuild configuration

mark_as_advanced( BUILD_SHARED_LIBS )
option( BUILD_SHARED_LIBS "Build shared libraries." OFF )

mark_as_advanced( BUILD_EXPRESS )
option( BUILD_EXPRESS "" OFF )

# Build release by default
if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

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

# ---------------------------------------------------------------------
# Build ITK

set( ITK_VERSION_MAJOR "4" )
set( ITK_VERSION_MINOR "10" )
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
# Build Elastix

mark_as_advanced( USE_SYSTEM_ELASTIX )
option( USE_SYSTEM_ELASTIX "Use an installed version of elastix" OFF )
if( USE_SYSTEM_ELASTIX )
  if( NOT EXISTS ${ELASTIX_USE_FILE} )
    set( ELASTIX_USE_FILE "NOT_FOUND" CACHE PATH "Absolute path to UseElastix.cmake." )
    message( FATAL_ERROR "Could not find the ELASTIX_USE_FILE (${ELASTIX_USE_FILE}). Use the SuperBuild or manually point the ELASTIX_USE_FILE CMake variable to the UseElastix.cmake file in source directory of your elastix build tree." )
  endif()
else()
  include( ExternalElastix )
endif()

# ---------------------------------------------------------------------
# Build Niftyreg

mark_as_advanced( SUPERELASTIX_BUILD_NIFTYREG )
option( SUPERELASTIX_BUILD_NIFTYREG "Build support for Niftyreg." ON )
if( SUPERELASTIX_BUILD_NIFTYREG )
#TODO add USE_SYSTEM_NIFTYREG option
  include( ExternalNiftyreg )
endif()

# ---------------------------------------------------------------------
# Build Boost

mark_as_advanced( USE_SYSTEM_BOOST )
option( USE_SYSTEM_BOOST "Use an installed version of BOOST" OFF )
if( USE_SYSTEM_BOOST )
  find_package( BOOST REQUIRED log graph program_options filesystem )
else()
  include( ExternalBoost )
endif()

# ---------------------------------------------------------------------
# Build GoogleTest

mark_as_advanced( USE_SYSTEM_GOOGLETEST )
option( USE_SYSTEM_GOOGLETEST "Use a pre-compiled version of GoogleTest. " OFF ) 

if ( USE_SYSTEM_GOOGLETEST ) 
  find_package( GTest REQUIRED ) 
else() 
  include( ExternalGoogleTest ) 
endif() 

# ---------------------------------------------------------------------
# Build SuperElastix

include( SuperElastix )

# ---------------------------------------------------------------------
# Build Applications as an external project

if( ${BUILD_APPLICATIONS_EXTERNAL_PROJECT} )
  include( ExternalApplications )
endif()



