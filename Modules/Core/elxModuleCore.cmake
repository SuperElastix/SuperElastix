set( ${MODULE}_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Common/include
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/include
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Install/include
)

set( ${MODULE}_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/src/elxComponentDescriptor.cxx
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/src/elxBlueprint.cxx
  ${CMAKE_SOURCE_DIR}/${MODULE_PATH}/Blueprints/src/itkFactory.cxx
)

set( ${MODULE}_LIBRARIES 
  elxModuleCore
)