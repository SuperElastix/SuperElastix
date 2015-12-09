set( MODULE elxModuleCore )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/include
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/include
)

file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/*/include/*.*")
#@Kasper: file GLOB is generally disencouraged for source files (missing files are not detected and CMake doesn't know if the project has to be updated when due to a checkout extra source files are in the tree), but since this I think is not really an issue for header files...
#I read: CMake will figure out that they're headers; it won't try to build them. http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project



# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/elxBlueprint.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/ComponentBase.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/Example3rdPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/Example4thPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/GDOptimizer3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/GDOptimizer4thPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/Metric3rdPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/Metric4thPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/SSDMetric3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/SSDMetric4thPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/TransformComponent1.cxx
  ${${MODULE}_SOURCE_DIR}/ExampleComponents/src/MetricComponent1.cxx
)


# Compile library

add_library( ${MODULE} STATIC "${${MODULE}_SOURCE_FILES}" ${${MODULE}_HEADER_FILES})
target_link_libraries( ${MODULE} ${ELASTIX_LIBRARIES} )
