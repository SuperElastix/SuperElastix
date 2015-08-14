# ---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest and
# the elastix dashboard. 

set( ElastixUnitTestFilenames
  elxExampleUnitTest.cxx
  itkRegistration.cxx
)

# ---------------------------------------------------------------------
# Set data directories
set( ELASTIX_UNITTEST_INPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Input )
set( ELASTIX_UNITTEST_OUTPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Output )
set( ELASTIX_UNITTEST_BASELINE_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Baseline )

if( NOT EXISTS ${ELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
  file( MAKE_DIRECTORY ${ELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
endif()

if( NOT EXISTS ${ELASTIX_UNITTEST_OUTPUT_DATA_DIR} )
  message( FATAL_ERROR 
    "Could not create directory for output data. Make sure elastix has permission to write to disk."
  )
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxDataDirectories.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/elxDataDirectories.h
  ESCAPE_QUOTES
)

# Build tests
add_library( DataManager elxDataManager.cxx )

list( APPEND TEST_LIBRARIES 
  DataManager
)

foreach( ElastixUnitTestFilename ${ElastixUnitTestFilenames} )
  # Build tests executables
  string( REPLACE ".cxx" "" ElastixUnitTest ${ElastixUnitTestFilename} )
  add_executable( "${ElastixUnitTest}" ${ElastixUnitTestFilename} )
  target_link_libraries( "${ElastixUnitTest}" ${ELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  ExternalData_Add_Test( ElastixData
    NAME ${ElastixUnitTest}
    COMMAND ${ElastixUnitTest} "--gtest_output=xml:${CMAKE_BINARY_DIR}/Testing/Unit/${ElastixUnitTest}.xml"
  )
endforeach()
