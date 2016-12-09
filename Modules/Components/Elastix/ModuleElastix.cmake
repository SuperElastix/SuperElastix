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

# If OpenMP is supported by this machine, elastix will be compiled with
# OpenMP flags, and we need to add them here as well
find_package( OpenMP QUIET )
if (OPENMP_FOUND)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
endif()

# Find UseElastix.cmake 
if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_USE_FILE ${ELASTIX_DIR}/UseElastix.cmake )
endif()

if( NOT EXISTS ${ELASTIX_USE_FILE} )
  set( ELASTIX_DIR "" CACHE PATH "Path to elastix build folder" )
  message(FATAL_ERROR "Could not find UseElastix.cmake. Point ELASTIX_DIR to folder containing UseElastix.cmake or use SuperBuild.")
endif()

# TODO: Add include and link directories manually to avoid elastix polluting CMake environment
include( ${ELASTIX_USE_FILE} )

set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)

set( ${MODULE}_SOURCE_FILES
)

set( ${MODULE}_TEST_SOURCE_FILES 
  ${${MODULE}_SOURCE_DIR}/test/selxElastixComponentTest.cxx
)

set( ${MODULE}_LIBRARIES 
  ${MODULE}
  elastix
  transformix
)
