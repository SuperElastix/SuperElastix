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

set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/Common/include
  ${${MODULE}_SOURCE_DIR}/Blueprints/include
  ${${MODULE}_SOURCE_DIR}/ParameterObject/include
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/include
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/include
  ${${MODULE}_SOURCE_DIR}/Filter/include
  ${${MODULE}_SOURCE_DIR}/FileIO/include
  ${${MODULE}_SOURCE_DIR}/ComponentRegistration/include
)

set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/selxBlueprint.cxx
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/selxBlueprintImpl.h
  ${${MODULE}_SOURCE_DIR}/Blueprints/src/selxBlueprintImpl.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/selxComponentBase.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/selxOverlord.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/src/selxComponentSelector.cxx
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/src/selxConfigurationReader.cxx
  ${${MODULE}_SOURCE_DIR}/Filter/src/selxSuperElastixFilter.cxx
)

# Export tests
set( ${MODULE}_TEST_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/Blueprints/test/selxBlueprintTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxComponentFactoryTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxComponentInterfaceTest.cxx
  ${${MODULE}_SOURCE_DIR}/ComponentInterface/test/selxOverlordTest.cxx
  ${${MODULE}_SOURCE_DIR}/ConfigurationReader/test/selxConfigurationReaderTest.cxx
  ${${MODULE}_SOURCE_DIR}/Filter/test/selxSuperElastixFilterTest.cxx
  ${${MODULE}_SOURCE_DIR}/FileIO/test/selxAnyFileIOTest.cxx
)

set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

set( ${MODULE}_MODULE_DEPENDENCIES 
  ModuleController
  ModuleElastix
  ModuleExamples
  ModuleItkImageRegistrationMethodv4
  ModuleItkSmoothingRecursiveGaussianImageFilter
  ModuleSinksAndSources
  ModuleNiftyreg
)
