cmake_minimum_required( VERSION 2.8 )

#---------------------------------------------------------------------
project( elastix )

#---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxRequiredITKModules.cmake" )

#---------------------------------------------------------------------
# Build Elastix

#---------------------------------------------------------------------
# Testing

# Do not build tests by default since this requires GoogleTest submodule
# to be initialized. Otherwise users may see FATAL_ERRORs out of the box.
# The SuperBuild will correctly initialize the GoogleTest submodule and 
# build tests by default.
option( ELASTIX_BUILD_TESTING "Enable building tests." OFF )

if( ELASTIX_BUILD_TESTING )
  enable_testing()
  add_subdirectory( Testing )
endif()

#---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( ELASTIX_BUILD_DOXYGEN )
option( ELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( ELASTIX_BUILD_READTHEDOCS )
option( ELASTX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

