#---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest and
# the Kibana dashboard.

set( ElastixUnitTestFilenames
  elxExampleUnitTest.cxx
)

#---------------------------------------------------------------------
# Set data directories
set( ELASTIX_UNITTEST_INPUT_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Input )
set( ELASTIX_UNITTEST_BASELINE_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Data/Baseline )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxTestHarness.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/elxTestHarness.h
  ESCAPE_QUOTES
)

# Build tests
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
