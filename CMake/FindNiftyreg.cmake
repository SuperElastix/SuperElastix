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
 
#set(Niftyreg_FIND_COMPONENTS lib_reg_aladin lib_reg_blockMatching lib_reg_f3d lib_reg_femTrans lib_reg_globalTrans lib_reg_localTrans lib_reg_maths lib_reg_measure  lib_reg_resampling lib_reg_tools)
set(Niftyreg_FIND_COMPONENTS _reg_aladin _reg_blockMatching _reg_f3d _reg_femTrans _reg_globalTrans _reg_localTrans _reg_maths _reg_measure _reg_resampling _reg_tools)

set(Niftyreg_INCLUDE_DIR "${NIFTYREG_DIR}/include")

foreach(component ${Niftyreg_FIND_COMPONENTS})
  #set(Niftyreg_LIBRARIES ${Niftyreg_LIBRARIES}  ${Niftyreg_DIR}/lib/${component}.a)
  set(Niftyreg_LIBRARIES ${Niftyreg_LIBRARIES}  ${Niftyreg_DIR}/lib/${component}.lib)
endforeach()

# handle the QUIETLY and REQUIRED arguments and set Niftyreg_FOUND to TRUE if
# all listed variables are TRUE
#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Niftyreg DEFAULT_MSG Niftyreg_LIBRARIES Niftyreg_INCLUDE_DIR)

mark_as_advanced(
  Niftyreg_INCLUDE_DIR
  Niftyreg_LIBRARIES
)