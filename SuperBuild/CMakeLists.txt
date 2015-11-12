cmake_minimum_required( VERSION 2.8 )

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
# Elastix SuperBuild configuration

# Build release by default
if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

# Build examples by default
# Examples will be build as an external project to verify the installation of elastix
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
# Boost Graph Library

mark_as_advanced( USE_SYSTEM_BOOST )
option( USE_SYSTEM_BOOST "Use an installed version of BOOST" OFF )
if( USE_SYSTEM_BOOST )
  find_package( BOOST REQUIRED graph )
else()
  include( ExternalBoost )
endif()

# ---------------------------------------------------------------------
# Build Elastix

# TODO: Add USE_SYSTEM_ELASTICLEGACY option
mark_as_advanced( SUPERELASTIX_BUILD_ELASTIX )
option( SUPERELASTIX_BUILD_ELASTIX ON )
if( SUPERELASTIX_BUILD_ELASTIX )
  include( ExternalElastix )
endif()

# ---------------------------------------------------------------------
# Build SuperElastix

include( SuperElastix )



