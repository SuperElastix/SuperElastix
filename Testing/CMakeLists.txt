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
# Find GoogleTest library

find_package( GTest REQUIRED )
include_directories( ${GTEST_INCLUDE_DIRS} )

set( TEST_LIBRARIES
  ${GTEST_BOTH_LIBRARIES}
)

# ---------------------------------------------------------------------
# Build test suite

set( CTEST_CONFIGURATION_TYPE Debug )

# Unit tests
add_subdirectory( Unit )

# ---------------------------------------------------------------------
# Configure dashboard
mark_as_advanced( ELASTIX_BUILD_DASHBOARD )
option( ELASTIX_BUILD_DASHBOARD "Enable elastix dashboard." OFF )

if( ${ELASTIX_BUILD_DASHBOARD} )
  add_subdirectory( Dashboard )
endif()
