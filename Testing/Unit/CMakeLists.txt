#---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest.

set( ElastixUnitTestFilenames
  elxExampleUnitTest.cxx
)

#---------------------------------------------------------------------

# Set test harness data directories
set( ELASTIX_TEST_DATA_INPUT_DIR ${CMAKE_CURRENT_DIR}/Data/Input )
set( ELASTIX_TEST_DATA_BASELINE_DIR ${CMAKE_CURRENT_DIR}/Data/Baseline )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/elxTestHarnessPaths.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/elxTestHarnessPaths.h
  ESCAPE_QUOTES
)

# Build tests executables
foreach( ElastixUnitTestFilename ${ElastixUnitTestFilenames} )
  string( REPLACE ".cxx" "" ElastixUnitTest ${ElastixUnitTestFilename} )
  add_executable( "${ElastixUnitTest}" ${ElastixUnitTestFilename} )
  target_link_libraries( "${ElastixUnitTest}" ${ELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  ExternalData_Add_Test( ElastixUnitTestData
    NAME ${ElastixUnitTest}
    COMMAND "${ElastixUnitTest}" -gtest_output
  )
endforeach()

