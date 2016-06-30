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

set( PROJECT GoogleBenchmark )
set( GOOGLEBENCHMARK_REPOSITORY https://github.com/google/benchmark.git )
set( GOOGLEBENCHMARK_TAG "v${GOOGLEBENCHMARK_VERSION_STRING")

ExternalProject_Add( ${PROJECT}
  GIT_REPOSITORY ${GOOGLEBENCHMARK_REPOSITORY}
  GIT_TAG ${GOOGLEBENCHMARK_TAG}
  UPDATE_COMMAND ""
  SOURCE_DIR ${PROJECT}
  BINARY_DIR ${prPROJECToj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DBENCHMARK_ENABLE_TESTING:BOOL=OFF
    -DCMAKE_SKIP_RPATH:BOOL=ON
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)
