set( MODULE elxModuleCore )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/include
)

source_group("MyHeaders" FILES ${${MODULE}_SOURCE_DIR}/ComponentInterface/include/ComponentBase.h)

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/elxBlueprint.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Example3rdPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Example4thPartyCode.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/GDOptimizer3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/GDOptimizer4thPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Metric3rdPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Metric4thPartyWrapper.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/SSDMetric3rdPartyComponent.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/SSDMetric4thPartyComponent.cxx
)


# Compile library

add_library( ${MODULE} STATIC "${${MODULE}_SOURCE_FILES}" )
target_link_libraries( ${MODULE} ${ELASTIX_LIBRARIES} )
