cmake_minimum_required( VERSION 2.8 )

# Explicitly add INCREMENTAL linking option to command lines.
# http://www.cmake.org/pipermail/cmake/2010-February/035174.html
set( MSVC_INCREMENTAL_DEFAULT ON )

# ---------------------------------------------------------------------
project( Elastix )

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
# Build Elastix

set( ELASTIX_COMMON_INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/Modules/Core/Common/include
)

set( ELASTIX_CORE_INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/Modules/Core/Blueprints/include
)

set( ELASTIX_INCLUDE_DIRECTORIES
  ${ELASTIX_COMMON_INCLUDE_DIRECTORIES}
  ${ELASTIX_CORE_INCLUDE_DIRECTORIES}
)

include_directories( ${ELASTIX_INCLUDE_DIRECTORIES} )
add_subdirectory( Modules )

# ---------------------------------------------------------------------
# Testing

# Testing requires CMake version 2.8.11 to download test data
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  set( ELASTIX_BUILD_TESTING_DEFAULT OFF )
  message( STATUS "ELASTIX_BUILD_TESTING is set to OFF because CMake version is less than 2.8.11" )
else()
  set( ELASTIX BUILD_TESTING_DEFAULT ON )
endif()

option( ELASTIX_BUILD_TESTING "Enable building tests." ${ELASTIX_BUILD_TESTING_DEFAULT} )
if( ${ELASTIX_BUILD_TESTING} )
  enable_testing()
  add_subdirectory( Testing )
endif()

# ---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( ELASTIX_BUILD_DOXYGEN )
option( ELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( ELASTIX_BUILD_READTHEDOCS )
option( ELASTIX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

