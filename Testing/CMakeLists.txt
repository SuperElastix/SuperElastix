#---------------------------------------------------------------------
# Setup GoogleTest

if( NOT EXISTS "${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git" )
  message( FATAL_ERROR "Could not find GoogleTest submodule. Please run 
git submodule update --init in the source directory to compile tests." )
endif()

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
  include( ${CMAKE_SOURCE_DIR}/CMake/elxGoogleTestWinConfig.cmake )
endif()

add_subdirectory( GoogleTest )

include_directories(
  ${gtest_SOURCE_DIR}/include
)

set( TEST_LIBRARIES
  gtest_main
)

#---------------------------------------------------------------------
# Setup ExternalData

# ExternalData module was introduced in CMake version 2.8.11
cmake_minimum_required( VERSION 2.8.11 )

include( ExternalData )

list( APPEND ExternalData_URL_TEMPLATES
  "http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
  "https://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
)

#---------------------------------------------------------------------
# Include tests
add_subdirectory( Unit )

# Include benchmarks
if( ${ELASTIX_BUILD_BENCHMARKING} )
  add_subdirectory( Benchmark )
endif()
