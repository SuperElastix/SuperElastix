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

set( proj SuperElastix )

UPDATE_SELX_SUPERBUILD_COMMAND(${proj})

ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
  BINARY_DIR ${proj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_CONFIGURATION_TYPES:STRING=${CMAKE_CONFIGURATION_TYPES}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DBUILD_EXAMPLES:BOOL=${BUILD_EXAMPLES}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -DBUILD_INTEGRATION_TESTS:BOOL=${BUILD_INTEGRATION_TESTS}
    -DBUILD_LONG_UNIT_TESTS:BOOL=${BUILD_LONG_TESTS}
    -DSuperElastixSuperBuild_DIR:PATH=${PROJECT_BINARY_DIR}
  DEPENDS ${SUPERELASTIX_DEPENDENCIES}
  BUILD_COMMAND ${SELX_SUPERBUILD_COMMAND}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1 
)

ExternalProject_Get_Property( ${proj} binary_dir )
set( SuperElastix_DIR "${binary_dir}" )

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
