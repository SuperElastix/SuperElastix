# ---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest and
# the elastix dashboard. 

# Not compiling elastix tests while we debug windows build. See minimum viable examples below
# set( ElastixUnitTestFilenames
#   elxExampleUnitTest.cxx
#   elxBlueprintTest.cxx
# )         

# Pass
add_executable( HelloWorld HelloWorld.cxx)

# Pass
add_executable( HelloWorldBlueprint HelloWorldBlueprint.cxx )
target_link_libraries( HelloWorldBlueprint ${ELASTIX_LIBRARIES} ${ITK_LIBRARIES} )

# Fail. It seems GoogleTest is compiled statically (like it should), while the executable is compiled dynamically (it should also be static) Why? BUILD_SHARED_LIBS is set to OFF!

# And why is the static linker option not recongnized (VS2013)?
set(CMAKE_EXE_LINKER_FLAGS /MTd ) 

add_executable( HelloWorldGoogleTest HelloWorldGoogleTest.cxx )
target_link_libraries( HelloWorldGoogleTest ${TEST_LIBRARIES} )

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
