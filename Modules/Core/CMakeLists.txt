project( ELXMODULE_CORE )

# TODO: Include directures using the module API
include_directories( 
  Common/include
  Blueprints/include
)

add_subdirectory( Common )
add_subdirectory( Blueprints )

set( "${PROJECT_NAME}_LIBRARIES"
  Blueprints
)

