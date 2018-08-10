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
#
# This CMakeLists file will generate the file selxCompiledLibraryComponents.h
# in the bin directory. This file defines the components to be compiled.
#

set( MODULE_COMPONENT_GROUP_INCLUDE_FILES ) 
set( MODULE_COMPONENT_TYPELISTS_ )
set( MODULE_COMPONENT_TYPELISTS )

foreach( MODULE ${SUPERELASTIX_MODULES} )
  if( ${USE_${MODULE}} )
    #TODO: now these modules are hardcode to be excluded, but they should not in this list in the first place
    if ( ${MODULE} STREQUAL "ModuleFilter" OR ${MODULE} STREQUAL "ModuleCore" OR ${MODULE} STREQUAL "ModuleBlueprints" OR ${MODULE} STREQUAL "ModuleLogger" OR ${MODULE} STREQUAL "ModuleFileIO" OR ${MODULE} STREQUAL "ModuleCommon")
    else()
      # building the strings:
      set( MODULE_COMPONENT_GROUP_INCLUDE_FILES "${MODULE_COMPONENT_GROUP_INCLUDE_FILES}#include \"selx${MODULE}.h\"\n" )
      set( MODULE_COMPONENT_TYPELISTS_ "${MODULE_COMPONENT_TYPELISTS_}  ${MODULE}Components,\n")
    endif()
  endif()
endforeach()

# Lots of cmake code just to remove the last comma and \n from the string
if( MODULE_COMPONENT_TYPELISTS_ )
  string(LENGTH ${MODULE_COMPONENT_TYPELISTS_} LEN_WITH_LAST_COMMA )
  math(EXPR LEN_WITHOUT_LAST_COMMA "${LEN_WITH_LAST_COMMA}-2" ) 
  string( SUBSTRING ${MODULE_COMPONENT_TYPELISTS_} 0 ${LEN_WITHOUT_LAST_COMMA} MODULE_COMPONENT_TYPELISTS )
  unset( LEN_WITH_LAST_COMMA )
  unset( LEN_WITHOUT_LAST_COMMA )
else()
  set( MODULE_COMPONENT_TYPELISTS ${MODULE_COMPONENT_TYPELISTS_} )
endif()
unset( MODULE_COMPONENT_TYPELISTS_ )

configure_file(
  ${ModuleFilter_SOURCE_DIR}/include/selxCompiledLibraryComponents.h.in
  ${PROJECT_BINARY_DIR}/selxCompiledLibraryComponents.h
  @ONLY
)
