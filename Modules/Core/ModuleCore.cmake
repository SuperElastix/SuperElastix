#=========================================================================
#
#  Copyright Leiden University Medical Center, Erasmus University Medical 
#  Center and contributors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#=========================================================================

set( MODULE ModuleCore )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
  ${${MODULE}_SOURCE_DIR}/ParameterObject/include
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/include
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/include
  ${${MODULE}_SOURCE_DIR}/Filter/include
  ${${MODULE}_SOURCE_DIR}/FileIO/include
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
  ${${MODULE}_SOURCE_DIR}/Filter/test/selxSuperElastixFilterTest.cxx
  ${${MODULE}_SOURCE_DIR}/FileIO/test/selxAnyFileIOTest.cxx
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/selxBlueprint.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/ComponentBase.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/Overlord.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/ComponentSelector.cxx
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/src/selxConfigurationReader.cxx
  ${${MODULE}_SOURCE_DIR}/Filter/src/selxSuperElastixFilter.cxx
)

# Compile library
add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES} )
target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} )
