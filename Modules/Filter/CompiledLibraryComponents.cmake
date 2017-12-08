#
# This CMakeLists file will generate the file selxDefaultComponents.h
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
      set( MODULE_COMPONENT_GROUP_INCLUDE_FILES "${MODULE_COMPONENT_GROUP_INCLUDE_FILES}#include \"selx${MODULE}.h\"\n" )
      set( MODULE_COMPONENT_TYPELISTS_ "${MODULE_COMPONENT_TYPELISTS_}  ${MODULE}Components,\n")
    endif()
  endif()
endforeach()

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
  ${ModuleFilter_BINARY_DIR}/include/selxCompiledLibraryComponents.h
  @ONLY
)
