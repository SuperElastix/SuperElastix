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
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest.

set( ElastixUnitTestSource
  elxExampleTest.cxx
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
# To add data to a tests, create a list "${filename}Data" (without
# the filename extension) and add data to this list.

set( elxExampleTestData
  DATA{Data/Input/BrainProtonDensitySliceR10X13Y17.png}
)

#---------------------------------------------------------------------
# Build test executables

foreach( ElastixUnitTestFilename ${ElastixUnitTestSource} )
  string( REPLACE ".cxx" "" ElastixUnitTest ${ElastixUnitTestFilename} )
  add_executable( ${ElastixUnitTest} ${ElastixUnitTestFilename} )
  target_link_libraries( ${ElastixUnitTest} ${ELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  ExternalData_Add_Test( "${ElastixUnitTest}ExternalDataTarget"
    NAME ${ElastixUnitTest}
    COMMAND ${ElastixUnitTest} ${${ElastixUnitTest}Data}
  )

  ExternalData_Add_Target( "${ElastixUnitTest}ExternalDataTarget"} )
endforeach()

