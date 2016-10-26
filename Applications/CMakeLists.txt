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

cmake_minimum_required( VERSION 3.0.2 )
project(SuperElastixExamples)

if( NOT CMAKE_PROJECT_NAME STREQUAL "SuperElastix" )
  # Examples are build as an external project, need to import dependencies

  set(CMAKE_CXX_STANDARD 11)
  
  find_package( SuperElastix )
  include( ${SUPERELASTIX_USE_FILE} )

  find_package( ITK )
  include( ${ITK_USE_FILE} )

  message( STATUS "ELASTIX_USE_FILE: ${ELASTIX_USE_FILE}")
  include( ${ELASTIX_USE_FILE} )
endif()

add_subdirectory( CommandlineProgram )