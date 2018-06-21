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

# Directories relative to the build directory.
set(CTEST_SOURCE_DIRECTORY "../../src")
set(CTEST_BINARY_DIRECTORY ".")

set(CTEST_SITE "lkeb-selx01")

if(NOT DEFINED CTEST_GIT_COMMAND)
  find_program(CTEST_GIT_COMMAND NAMES git git.cmd)
endif()
set(CTEST_GIT_UPDATE_CUSTOM "${CTEST_GIT_COMMAND}" pull origin)

if ("$ENV{CHANGE_AUTHOR}" STREQUAL "")
  set(CTEST_BUILD_NAME "$ENV{BRANCH_NAME}")
else()
  set(CTEST_BUILD_NAME "$ENV{BRANCH_NAME};$ENV{CHANGE_AUTHOR}")
endif()
set(CTEST_BUILD_NAME "${CTEST_BUILD_NAME};Test")


set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_CONFIGURATION Release)
set(CTEST_BUILD_FLAGS "-j4")

string(CONCAT CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND}"
    " -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION}"
    " --build ${CTEST_BINARY_DIRECTORY}"
    " ${CTEST_BUILD_OPTIONS}"
    " \"-G${CTEST_CMAKE_GENERATOR}\""
    " \"${CTEST_SOURCE_DIRECTORY}\"")

# Tells CTest to not do a git pull, but to still record what version of the software it's building and testing
# As explained by mail, by Zack Galbreath
set(CTEST_UPDATE_VERSION_ONLY 1)

# For CDash integration with GitHub: https://blog.kitware.com/cdash-integration-with-github
set(CTEST_CHANGE_ID $ENV{CHANGE_ID})

ctest_start("Continuous")
# Added ctest_update() to ensure that the commit SHA will be passed to CDash, and GitHub.
ctest_update()
ctest_test(RETURN_VALUE RES)
ctest_submit( PARTS Test Update )

if (RES)
  message(FATAL_ERROR "Unit tests have return code != 0")
else (RES)
  message(STATUS "Unit tests have return code ${RES}")
endif(RES)
