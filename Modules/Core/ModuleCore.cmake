set( MODULE ModuleCore )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
  ${${MODULE}_SOURCE_DIR}/ParameterObject/include
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/include
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/include
  )

# Collect header files for Visual Studio Project 
# http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/*/include/*.*")

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

# Export tests
set( ${MODULE}_TESTS
  ${${MODULE}_SOURCE_DIR}/Blueprints/test/selxBlueprintTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxComponentFactoryTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxComponentInterfaceTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxOverlordTest.cxx
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/test/selxConfigurationReaderTest.cxx
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/selxBlueprint.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/ComponentBase.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Overlord.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/ComponentSelector.cxx
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/src/selxConfigurationReader.cxx
)

# Compile library
add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES} )
target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} )
