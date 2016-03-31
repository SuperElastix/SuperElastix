# Module that exposes old elastix as an ITK filter
set( MODULE ModuleElastix )

# If OpenMP is supported, elastix will have been compiled with
# OpenMP flags, and we need to add them here as well
find_package( OpenMP )
if( OPENMP_FOUND )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
endif()

# Find UseElastix.cmake 
if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_USE_FILE ${ELASTIX_DIR}/UseElastix.cmake )
endif()

if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_DIR "" CACHE PATH "Path to elastix build folder" )
  message(FATAL_ERROR "Could not find UseElastix.cmake. Point ELASTIX_DIR to folder containing UseElastix.cmake or use SuperBuild.")
endif()

# Export include files
include( ${ELASTIX_USE_FILE} )

# Export libraries
set( ${MODULE}_LIBRARIES 
  elastix
  transformix
)

# Export tests
set( ${MODULE}_TESTS 
  elxElastixFilterTest.cxx
  elxTransformixFilterTest.cxx
)
