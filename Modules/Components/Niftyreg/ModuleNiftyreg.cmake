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
#find_package( OpenMP )
#if (OPENMP_FOUND)
#  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
#  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
#endif()

find_path(Niftyreg_DIR "_reg_f3d.h" HINTS ${CMAKE_BUILD_DIR})

if( Niftyreg_DIR-NOTFOUND )
  message(
    FATAL_ERROR
    "Niftyreg_DIR-NOTFOUND"
	"Looked in:" ${CMAKE_BUILD_DIR}
  )
endif()

find_library(Niftyreg lib_reg_aladin lib_reg_blockMatching lib_reg_f3d lib_reg_femTrans lib_reg_globalTrans lib_reg_localTrans lib_reg_maths lib_reg_measure  lib_reg_resampling lib_reg_tools libreg_nifti libreg_png lib_reg_ReadWriteImage HINTS ${CMAKE_BUILD_DIR})

if( Niftyreg-NOTFOUND )
  message(
    FATAL_ERROR
    "Niftyreg-NOTFOUND"
	"Looked in:" ${CMAKE_BUILD_DIR}
  )
endif()



# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
  ${Niftyreg_DIR}
)

# Collect header files for Visual Studio Project
file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/include/*.*")

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
  lib_reg_f3d
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

target_link_libraries( ${MODULE} ${Niftyreg_LIBRARIES} )
