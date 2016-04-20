set( MODULE ModuleItkImageRegistrationMethodv4 )

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
  ${${MODULE}_SOURCE_DIR}/test/selxRegistrationItkv4Test.cxx
  ${${MODULE}_SOURCE_DIR}/test/selxWBIRDemoTest.cxx
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageRegistrationMethodv4.cxx 
  )

# Compile library

add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES})

target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} )
