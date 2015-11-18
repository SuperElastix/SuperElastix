# Module that exposes the old elastix as a component
set( MODULE ModuleElastix )

if( NOT EXISTS ${ELASTIX_USE_FILE} )
  message( FATAL_ERROR "${MODULE} could not find ELASTIX_USE_FILE. Use the SuperBuild or manually point the ELASTIX_USE_FILE CMake variable to the UseElastix.cmake file in the root of your elastix build tree." )
endif()

include( ${ELASTIX_USE_FILE} )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)
