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

set( ${APPLICATION}_TARGET_NAME SuperElastix )

set( ${APPLICATION}_INCLUDE_DIRS 
  ${${APPLICATION}_SOURCE_DIR}/include
)
set( ${APPLICATION}_SOURCE_FILES
  ${${APPLICATION}_SOURCE_DIR}/src/selxSuperElastix.cxx
)

set( ${APPLICATION}_MODULE_DEPENDENCIES
  ModuleFilter
)

set( ${APPLICATION}_INTEGRATION_TEST_SOURCE_FILES 
  ${${APPLICATION}_SOURCE_DIR}/test/integration.cmake
)


# Originally based on the answer by Naszta at
# https://stackoverflow.com/questions/6526451/how-to-include-git-commit-number-into-a-c-executable
find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE SELX_GIT_REVISION_SHA
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  else()
    set(SELX_GIT_REVISION_SHA 0)
endif()


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Applications/selxGitRevisionSha.h.in ${CMAKE_CURRENT_BINARY_DIR}/Applications/selxGitRevisionSha.h @ONLY)
