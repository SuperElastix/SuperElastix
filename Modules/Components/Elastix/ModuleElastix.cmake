# Module that exposes old elastix as an ITK filter
set( MODULE ModuleElastix )

if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_USE_FILE )
  message( FATAL_ERROR "${MODULE} could not find ELASTIX_USE_FILE. Use the SuperBuild or manually point the ELASTIX_USE_FILE CMake variable to the UseElastix.cmake file in the root of your elastix build tree." )
endif()

include( ${ELASTIX_USE_FILE} )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)

# Export libraries
set( ${MODULE}_LIBRARIES 
  elastix
  transformix
)
