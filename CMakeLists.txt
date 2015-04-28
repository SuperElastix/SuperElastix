cmake_minimum_required( VERSION 2.8 )

#---------------------------------------------------------------------
project( elastix )

#---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxRequiredITKModules.cmake" )

#---------------------------------------------------------------------
# Testing

if( BUILD_TESTING )
  if( NOT EXISTS "${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git" )
    message( FATAL_ERROR "Could not find GoogleTest submodule. Please run git submodule init to compile tests." )
  endif()

  enable_testing()
  
  mark_as_advanced( BUILD_CTESTS )
  option( BUILD_CTESTS "Enable CTests." ON )
  if( BUILD_CTESTS )
    include( CTest )
  endif()

  add_subdirectory( Testing )
endif()

#---------------------------------------------------------------------
# Build Elastix

#---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( BUILD_DOXYGEN )
option( BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( BUILD_READTHEDOCS OFF )
option( BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

