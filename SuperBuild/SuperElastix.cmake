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

ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
  BINARY_DIR ${proj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DBUILD_EXAMPLES:BOOL=${BUILD_EXAMPLES}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -DBUILD_INTEGRATION_TESTS:BOOL=${BUILD_INTEGRATION_TESTS}
    -DBUILD_LONG_UNIT_TESTS:BOOL=${BUILD_LONG_TESTS}
    -DITK_DIR:PATH=${ITK_DIR}
    -DBOOST_ROOT:PATH=${BOOST_ROOT}
    -DGTEST_ROOT:PATH=${GTEST_ROOT}
    -DELASTIX_USE_FILE:PATH=${ELASTIX_USE_FILE}
  DEPENDS ${SUPERELASTIX_DEPENDENCIES}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property( SuperElastix binary_dir )
set( SuperElastix_DIR "${binary_dir}" )

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
