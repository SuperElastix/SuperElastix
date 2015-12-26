# Module that exposes old elastix as an ITK filter
set( MODULE ModuleElastix )

if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_USE_FILE )
  message( FATAL_ERROR "${MODULE} could not find ELASTIX_USE_FILE. Use the SuperBuild or manually point the ELASTIX_USE_FILE CMake variable to the UseElastix.cmake file in the root of your elastix build tree." )
endif()

include( ${ELASTIX_USE_FILE} )

# If OpenMP is supported, elastix will have beeen compiled with
# OpenMP flags, and we need to add them here as well
find_package( OpenMP )
if( OPENMP_FOUND )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
endif()

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)

# Export libraries
set( ${MODULE}_LIBRARIES 
  elastix
  transformix
)
