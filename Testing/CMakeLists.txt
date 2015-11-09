# ---------------------------------------------------------------------
# Download test data

# ExternalData module requires CMake version 2.8.11
if( CMAKE_VERSION VERSION_LESS 2.8.11 )
  message( FATAL_ERROR "Building tests require CMake 2.8.11 or newer." )
endif()

include( ExternalData )

list( APPEND ExternalData_URL_TEMPLATES
  "http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
  "https://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
)

# Scan the Data directory for content links on the form "[filename].[ext].md5". 
# The content links contains md5 hashes that are checked for consistensy  
# against files downloaded from servers in ExternalData_URL_TEMPLATES. These 
# files are placed in a corresponding location in the build directory.
file( GLOB_RECURSE ElastixDataContentLinks "*.md5" )
foreach(ElastixDataContentLink ${ElastixDataContentLinks})
  string( REGEX REPLACE "\\.md5$" "" ElastixDataContentLink ${ElastixDataContentLink} )
  ExternalData_Expand_Arguments( ElastixData
    ElastixDataFilenames
    DATA{${ElastixDataContentLink}}
  )
endforeach()

# Test data is downloaded when this target is built
ExternalData_Add_Target( ElastixData )

# ---------------------------------------------------------------------
# Add GoogleTest

find_package( Git )
if( NOT EXISTS "${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git" AND GIT_EXECUTABLE )
  execute_process( 
  	COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_QUIET
  )
endif()

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/Testing/GoogleTest/.git )
  message( 
  	FATAL_ERROR "Could not find GoogleTest submodule. Please run 
  	git submodule update --init in the source directory to compile tests."
  )
endif()

add_subdirectory( GoogleTest )

include_directories(
  ${gtest_SOURCE_DIR}/include
)

set( TEST_LIBRARIES
  gtest_main
)

# ---------------------------------------------------------------------
# Build test suite

# Unit tests
add_subdirectory( Unit )

# ---------------------------------------------------------------------
# Configure dashboard
mark_as_advanced( ELASTIX_BUILD_DASHBOARD )
option( ELASTIX_BUILD_DASHBOARD "Enable elastix dashboard." OFF )

if( ${ELASTIX_BUILD_DASHBOARD} )
  add_subdirectory( Dashboard )
endif()
