cmake_minimum_required( VERSION 2.8 )

# Explicitly add INCREMENTAL linking option to command lines.
# http://www.cmake.org/pipermail/cmake/2010-February/035174.html
set( MSVC_INCREMENTAL_DEFAULT ON )

# ---------------------------------------------------------------------
project( Elastix )

# Place libraries and executables in the bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/bin"
)

# Include SuperElastix CMake scripts
set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake" 
  ${CMAKE_MODULE_PATH}
)

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
  include( elxWinConfig.cmake )
endif()

# ---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxITKRequiredModules.cmake" )

# ---------------------------------------------------------------------
# Boost Graph Library
find_package( Boost REQUIRED graph )
include_directories( ${Boost_INCLUDE_DIRS} )

# ---------------------------------------------------------------------
# Build SuperElastix
# For now we just enable all modules
include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxModules.cmake" )
elxmodule_enable( elxModuleCore )

# ---------------------------------------------------------------------
# Testing

# Testing requires CMake version 2.8.11 to download test data
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  set( SUPERELASTIX_BUILD_TESTING_DEFAULT OFF )
  message( STATUS "ELASTIX_BUILD_TESTING is set to OFF because CMake version is less than 2.8.11" )
else()
  set( SUPERELASTIX_BUILD_TESTING_DEFAULT ON )
endif()

option( SUPERELASTIX_BUILD_TESTING "Enable building tests." ${ELASTIX_BUILD_TESTING_DEFAULT} )
if( ${ELASTIX_BUILD_TESTING} )
  enable_testing()
  add_subdirectory( Testing )
endif()

# ---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( SUPERELASTIX_BUILD_DOXYGEN )
option( SUPERELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( SUPERELASTIX_BUILD_READTHEDOCS )
option( SUPERELASTIX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

