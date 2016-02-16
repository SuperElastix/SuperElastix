set( MODULE ModuleSinksAndSources )

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
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSource.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageFilterSink.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSink.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSourceFixed.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSourceMoving.cxx
)

# Compile library

add_library( ${MODULE} STATIC "${${MODULE}_SOURCE_FILES}" ${${MODULE}_HEADER_FILES})

target_link_libraries( ${MODULE} ${ELASTIX_LIBRARIES} )

