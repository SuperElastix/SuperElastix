#---------------------------------------------------------------------
# To add a test to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest.

set( ElastixUnitTestSource
  elxExampleUnitTest.cxx
)

#---------------------------------------------------------------------
# To add data to a tests, create a list "${filename}Data" (without
# the filename extension) and add data to this list.

set( elxExampleTestData
  DATA{${CMAKE_BINARY_DIR}/Testing/Data/Input/BrainProtonDensitySliceR10X13Y17.png}
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