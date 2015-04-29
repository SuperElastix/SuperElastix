#---------------------------------------------------------------------
# Setup GoogleTest

if( NOT EXISTS "${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git" )
  message( FATAL_ERROR "Could not find GoogleTest submodule. Please run 
git submodule update --init in the source directory to compile tests." )
endif()

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
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
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest.

set( ElastixUnitTestSource
  elxExampleTest.cxx
)

#---------------------------------------------------------------------
# Build test executables

foreach( ElastixUnitTestFilename ${ElastixUnitTestSource} )
  string( REPLACE ".cxx" "" ElastixUnitTestExe ${ElastixUnitTestFilename} )
  add_executable( ${ElastixUnitTestExe} ${ElastixUnitTestFilename} )
  target_link_libraries( ${ElastixUnitTestExe} ${ELASTIX_LIBRARIES} ${TEST_LIBRARIES} )
  add_test( ElastixUnitTests ${ElastixUnitTestExe} )
endforeach()