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
 
set(Niftyreg_FIND_COMPONENTS _reg_aladin _reg_blockMatching _reg_f3d _reg_femTrans _reg_globalTrans _reg_localTrans _reg_maths _reg_measure _reg_ReadWriteImage _reg_resampling _reg_tools reg_nifti reg_png)

# *NIX OSes use system png and zlib, for Windows OS Niftyreg is configured to build these libraries
if( WIN32)
	list(APPEND Niftyreg_FIND_COMPONENTS png z)
endif (WIN32)

set(Niftyreg_INCLUDE_DIR ${NIFTYREG_DIR}/include CACHE PATH "Installed Include Directory of NiftyReg" )
#set(Niftyreg_LIBRARIES "" CACHE STRING "Found libraries NiftyReg" )
#message(STATUS NIFTYREG_DIR/lib "${NIFTYREG_DIR}/lib")
foreach(component ${Niftyreg_FIND_COMPONENTS})
#  message(component: ${component})
  find_library(Niftyreg_${component}_LIBRARY
    ${component}
    "${NIFTYREG_DIR}/lib"
    DOC "Path to NiftyReg library files (lib_*.a)"
  )
  set(Niftyreg_LIBRARIES ${Niftyreg_LIBRARIES} ${Niftyreg_${component}_LIBRARY})
endforeach()

# handle the QUIETLY and REQUIRED arguments and set Niftyreg_FOUND to TRUE if
# all listed variables are TRUE
#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Niftyreg DEFAULT_MSG Niftyreg_LIBRARIES Niftyreg_INCLUDE_DIR)
mark_as_advanced(
  Niftyreg_INCLUDE_DIR
  Niftyreg_LIBRARIES
)