cmake_minimum_required( VERSION 2.8 )

#---------------------------------------------------------------------
project( elastixSuperBuild )

find_package( Git REQUIRED )

include( ExternalProject )

set( CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../CMake
  ${CMAKE_MODULE_PATH}
)

#---------------------------------------------------------------------
# Elastix SuperBuild configuration

# Build release by default
if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

# Build examples by default
# Examples will be build as an external project to verify the installation of elastix
option( ELASTIX_BUILD_EXAMPLES "Enable building examples." ON )

# Build tests by default
option( ELASTIX_BUILD_TESTING "Enable building tests." ON )

if( ELASTIX_BUILD_TESTING )
  mark_as_advanced( ELASTIX_BUILD_CTESTS )
  option( ELASTIX_BUILD_CTESTS "Enable CTests." OFF )
endif()

#---------------------------------------------------------------------
# Build ITK as external project

set( ITK_VERSION_MAJOR "4" )
set( ITK_VERSION_MINOR "7" )
set( ITK_VERSION_PATCH "1" )
set( ITK_VERSION_STRING "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}" )

mark_as_advanced( USE_SYSTEM_ITK )
option( USE_SYSTEM_ITK "Use an installed version of ITK" OFF )

if( USE_SYSTEM_ITK )
  find_package( ITK ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR} REQUIRED )
  include( "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/elxRequiredITKModules.cmake" )
else()
  include( ExternalITK )
  list( APPEND ELASTIX_DEPENDENCIES ITK )
endif()

#---------------------------------------------------------------------
# Build Elastix as an external project

include( Elastix )



