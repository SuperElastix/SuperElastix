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

set( MODULE ModuleNiftyreg )

# If OpenMP is supported by this machine, niftyreg will be compiled with
# OpenMP flags, and we need to add them here as well
find_package( OpenMP )
if (OPENMP_FOUND)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
endif()

set(NIFTYREG_DIR "/home/flopper/Programming/SuperElastix/bld2/Niftyreg-prefix")
message(STATUS ${NIFTYREG_DIR})

find_package( Niftyreg )
#mark_as_advanced(Niftyreg_DIR)
if (NOT Niftyreg_FOUND)
  #set( NIFTYREG_DIR "" CACHE PATH "Path to Niftyreg build folder" )
  message(FATAL_ERROR "Could not find Niftyreg. Point NIFTYREG_DIR to its install folder.")
endif()

find_package( ZLIB )
# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
  ${Niftyreg_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIRS}
)

message(STATUS ${${MODULE}_INCLUDE_DIRS})

# Collect header files for Visual Studio Project
file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/include/*.*")

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
  #${Niftyreg_LIBRARIES}
)

# Export tests
set( ${MODULE}_TESTS 
  ${${MODULE}_SOURCE_DIR}/test/selxNiftyregComponentTest.cxx
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/selxNiftyregComponent.cxx 
  )

# Compile library

add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES})

target_link_libraries( ${MODULE} ${Niftyreg_LIBRARIES} ${ZLIB_LIBRARIES} )
