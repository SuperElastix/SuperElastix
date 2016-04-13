set( MODULE ModuleExamples )

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

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/Example3rdPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/src/Example4thPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/src/GDOptimizer3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/src/GDOptimizer4thPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/src/Metric3rdPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/src/Metric4thPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/src/SSDMetric3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/src/SSDMetric4thPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/src/TransformComponent1.cxx
  ${${MODULE}_SOURCE_DIR}/src/MetricComponent1.cxx
)

# Compile library

add_library( ${MODULE} STATIC "${${MODULE}_SOURCE_FILES}" ${${MODULE}_HEADER_FILES})

target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} )
