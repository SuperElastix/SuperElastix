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

set( proj ATen )
set( ATen_REPOSITORY https://github.com/pytorch/pytorch )
set( ATen_TAG 82e995e0b9bc73cedf98016b910297b44563342f )

ExternalProject_Add( ${proj}
  GIT_REPOSITORY ${ATen_REPOSITORY}
  GIT_TAG ${ATen_TAG}
  UPDATE_COMMAND ""
  SOURCE_DIR ${proj}
  SOURCE_SUBDIR aten
  BINARY_DIR ${proj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DBUILD_SHARED_LIBS:BOOL=${SUPERELASTIX_BUILD_SHARED_LIBS}
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_CONFIGURATION_TYPES:STRING=${CMAKE_CONFIGURATION_TYPES}
    -DNO_CUDA:BOOL=TRUE
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Get_Property( ${proj} install_dir )
set( ATEN_DIR "${install_dir}/share/cmake/ATen" )

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
