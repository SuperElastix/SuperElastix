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

set( proj Applications )

ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Applications
  BINARY_DIR ${proj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_CONFIGURATION_TYPES:STRING=${CMAKE_CONFIGURATION_TYPES}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DSuperElastixSuperBuild_DIR:PATH=${PROJECT_BINARY_DIR}
    -DSuperElastix_DIR:PATH=${SuperElastix_DIR}
    -DITK_DIR:PATH=${ITK_DIR}
  DEPENDS ${SUPERELASTIX_DEPENDENCIES}
  INSTALL_COMMAND ""
  BUILD_ALWAYS 1 
)
