set( MODULE ModuleElastixComponent )

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

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)

# Collect header files for Visual Studio Project
file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/include/*.*")

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

# Export tests
set( ${MODULE}_TESTS 
  elxElastixComponentTest.cxx
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/selxElastixComponent.cxx 
  )

# Compile library

add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES})

target_link_libraries( ${MODULE} ${ELASTIX_LIBRARIES} )
