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

# If OpenMP is supported by this machine, niftyreg will be compiled with
# OpenMP flags, and we need to add them here as well
find_package( OpenMP QUIET )
if (OPENMP_FOUND)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
endif()

find_package( Niftyreg REQUIRED )
if( NOT Niftyreg_FOUND )
  mark_as_advanced( NIFTYREG_DIR )
  set( NIFTYREG_DIR "" CACHE PATH "Path to Niftyreg build folder." )
  message( FATAL_ERROR "Could not find Niftyreg. Point NIFTYREG_DIR to its install folder." )
endif()


set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
  ${NIFTYREG_DIR}/include
)

# *NIX OSes use system png and zlib, for Windows OS Niftyreg is configured to build these libraries
#if (NOT WIN32)
#  find_package( PNG REQUIRED)
#  list(APPEND ${MODULE}_INCLUDE_DIRS ${PNG_INCLUDE_DIRS} )  
#  find_package( ZLIB REQUIRED)
#  list(APPEND ${MODULE}_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS} )
#endif(NOT WIN32)

set( ${MODULE}_SOURCE_FILES
  )

set( ${MODULE}_TEST_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/test/selxNiftyregComponentTest.cxx
  ${${MODULE}_SOURCE_DIR}/test/selxNiftiItkConversionsTest.cxx
  ${${MODULE}_SOURCE_DIR}/test/selxNiftyregDisplacementTest.cxx
)

set( ${MODULE}_LIBRARIES 
  ${PNG_LIBRARIES} 
  ${ZLIB_LIBRARIES}
  ${Niftyreg_LIBRARIES}
  ${Niftyreg__reg_ReadWriteImage_LIBRARY}
  ${Niftyreg__reg_f3d_LIBRARY}
)
