cmake_minimum_required( VERSION 2.8 )

# Include SuperElastix CMake scripts
list( APPEND CMAKE_MODULE_PATH 
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
)

if( CMAKE_HOST_WIN32 )
  include( elxWinConfig.cmake )
endif()

#---------------------------------------------------------------------
project( Elastix )

#---------------------------------------------------------------------
# ITK

find_package( ITK REQUIRED )
set( ITK_NO_IO_FACTORY_REGISTER_MANAGER 1 )
include( ${ITK_USE_FILE} )
include( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/elxITKRequiredModules.cmake" )

#---------------------------------------------------------------------
# Build Elastix

#---------------------------------------------------------------------
# Testing

# Build tests by default
option( ELASTIX_BUILD_TESTING "Enable building tests." ON )

# ExternalData module requires newer CMake versions
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  set( ELASTIX_BUILD_TESTING OFF FORCE )
  message( STATUS "ELASTIX_BUILD_TESTING was set to OFF because CMake version is less than 2.8.11")
endif()

if( ELASTIX_BUILD_TESTING )
  enable_testing()
  add_subdirectory( Testing )
endif()

#---------------------------------------------------------------------
# Build Documentation

mark_as_advanced( ELASTIX_BUILD_DOXYGEN )
option( ELASTIX_BUILD_DOXYGEN "Enable building Doxygen documentation." OFF )

mark_as_advanced( ELASTIX_BUILD_READTHEDOCS )
option( ELASTIX_BUILD_READTHEDOCS "Enable building readthedocs.org documentation." OFF )

