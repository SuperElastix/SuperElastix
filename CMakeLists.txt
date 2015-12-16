cmake_minimum_required( VERSION 2.8 )

# Explicitly add INCREMENTAL linking option to command lines.
# http://www.cmake.org/pipermail/cmake/2010-February/035174.html
#set( MSVC_INCREMENTAL_DEFAULT ON )

# ---------------------------------------------------------------------
project( Elastix )

enable_language(C)
enable_language(CXX)
set( CMAKE_CXX_STANDARD 11 )

# Place libraries and executables in the bin directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/bin"
)

# Include SuperElastix CMake scripts
list( APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" )


if( ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC )
  include( elxWinConfig )
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
include( elxModules )
elxmodule_enable( ModuleCore )
elxmodule_enable( ModuleElastix )

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

