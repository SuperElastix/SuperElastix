#
set( GTEST_OPTIONS
  -gtest_output
)

#---------------------------------------------------------------------
# To add a benchmark to the build system, append it to the list below. 
# Any GoogleTests in these files are automatically added to CTest.

set( ElastixBenchmarkSource
  elxExampleBenchmark.cxx
)

#---------------------------------------------------------------------
# To add data to a benchmark, create a list "${filename}Data" (without
# the filename extension) and add data to this list.

set( elxExampleBenchmarkData
  DATA{Testing/Data/Input/BrainProtonDensitySlice.png}
  DATA{Testing/Data/Input/BrainProtonDensitySliceR10X13Y17.png}
)

#---------------------------------------------------------------------
# Build executables

foreach( ElastixBenchmarkFilename ${ElastixBenchmarkSource} )
  string( REPLACE ".cxx" "" ElastixBenchmark ${ElastixBenchmarkFilename} )
  add_executable( ${ElastixBenchmark} ${ElastixBenchmarkFilename} )
  target_link_libraries( ${ElastixBenchmark} ${ELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  ExternalData_Add_Test( "${ElastixBenchmark}ExternalDataTarget"
    NAME ${ElastixBenchmark}
    COMMAND "${ElastixBenchmark} ${${ElastixBenchmarkTest}Data} ${GTEST_OPTIONS}"
  )

  ExternalData_Add_Target( "${ElastixBenchmark}ExternalDataTarget"} )
endforeach()
