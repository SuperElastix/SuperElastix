set( MODULE ModuleCore )

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/elxBlueprint.cxx
)

# Compile library
add_library( ${MODULE} STATIC "${${MODULE}_SOURCE_FILES}" )
target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
)

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)
