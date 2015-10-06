set( ${MODULE}_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Common/include
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/include
)

set( ${MODULE}_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/src/elxComponentDescriptor.cxx
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/src/elxBlueprint.cxx
)

set( ${MODULE}_LIBRARIES 
  elxModuleCore
)