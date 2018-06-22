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

set( proj Niftyreg )

set( NIFTYREG_GIT_REPOSITORY https://github.com/SuperElastix/niftyreg.git )
set( NIFTYREG_GIT_TAG 88b369becb6bdbda22ea96a0bef2b83bee85b219 )

UPDATE_SELX_SUPERBUILD_COMMAND(${proj})

ExternalProject_Add( ${proj} 
  GIT_REPOSITORY ${NIFTYREG_GIT_REPOSITORY}
  GIT_TAG ${NIFTYREG_GIT_TAG}
  TLS_VERIFY OFF
  UPDATE_COMMAND ""
  PATCH_COMMAND "${GIT_EXECUTABLE}" reset --hard && "${GIT_EXECUTABLE}" clean --force && "${GIT_EXECUTABLE}" apply --whitespace=fix "${CMAKE_SOURCE_DIR}/Patches/niftyreg_cpu_maths.patch" && "${GIT_EXECUTABLE}" apply --whitespace=fix "${CMAKE_SOURCE_DIR}/Patches/niftyreg_eigen_assignment.patch"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
   --no-warn-unused-cli
   -DBUILD_ALL_DEP:BOOL=ON
   -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
   -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
   -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
   -DCMAKE_CONFIGURATION_TYPES:STRING=${CMAKE_CONFIGURATION_TYPES}
   -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  BUILD_COMMAND ${SELX_SUPERBUILD_COMMAND}
)

ExternalProject_Get_Property( ${proj} install_dir )
set( NIFTYREG_DIR "${install_dir}")

list( APPEND SUPERELASTIX_DEPENDENCIES ${proj} )
