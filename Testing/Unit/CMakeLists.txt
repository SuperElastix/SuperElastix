# ---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest and
# the elastix dashboard. 

set( ElastixUnitTestFilenames
  elxBlueprintTest.cxx
  elxElastixFilterTest.cxx
  elxTransformixFilterTest.cxx
  elxComponentFactoryTest.cxx
  elxComponentInterfaceTest.cxx
)         

# ---------------------------------------------------------------------
# Options
if( SUPERELASTIX_BUILD_LONG_TESTS )
  add_definitions( -DSUPERELASTIX_BUILD_LONG_TESTS )
endif()

# ---------------------------------------------------------------------
# Set data directories
set( SUPERELASTIX_UNITTEST_INPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Input )
set( SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Output )
set( SUPERELASTIX_UNITTEST_BASELINE_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Baseline )

if( NOT EXISTS ${SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
  file( MAKE_DIRECTORY ${SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
endif()

if( NOT EXISTS ${SUPERELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
  message( FATAL_ERROR 
    "Could not create directory for output data. Make sure elastix has permission to write to disk."
  )
endif()

# ---------------------------------------------------------------------
# Build test data manager

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxDataDirectories.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/elxDataDirectories.h
  ESCAPE_QUOTES
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
add_library( DataManager elxDataManager.cxx )

list( APPEND TEST_LIBRARIES 
  DataManager
)

# ---------------------------------------------------------------------
# Build tests

foreach( SuperElastixUnitTestFilename ${SUPERELASTIX_TESTS} )
  # Build tests executables
  string( REPLACE ".cxx" "" SuperElastixUnitTest ${SuperElastixUnitTestFilename} )
  add_executable( ${SuperElastixUnitTest} ${SuperElastixUnitTestFilename} )
  target_link_libraries( "${SuperElastixUnitTest}" ${SUPERELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  # Add GoogleTest to CTest
  GTEST_ADD_TESTS( ${SuperElastixUnitTest} "--gtest_output=xml:${CMAKE_BINARY_DIR}/Testing/Unit/${SuperElastixUnitTest}.xml" ${SuperElastixUnitTestFilename} )
endforeach()
