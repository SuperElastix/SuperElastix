#
# This CMakeLists file will generate the file selxDefaultComponents.h
# in the bin directory. This file defines the components to be compiled.
#

set( MODULE_COMPONENT_GROUP_INCLUDE_FILES ) 
set( MODULE_COMPONENT_TYPELISTS_ )

foreach( MODULE ${SUPERELASTIX_MODULES} )
  if( ${USE_${MODULE}} )
    set( MODULE_COMPONENT_GROUP_INCLUDE_FILES "${MODULE_COMPONENT_GROUP_INCLUDE_FILES}#include \"selx${MODULE}.h\"\n" )
    set( MODULE_COMPONENT_TYPELISTS_ "${MODULE_COMPONENT_TYPELISTS_}  ${MODULE}Components,\n")
  endif()
endforeach()

string(LENGTH ${MODULE_COMPONENT_TYPELISTS_} LEN_WITH_COMMA)
math(EXPR LEN_WITHOUT_COMMA "${LEN_WITH_COMMA}-2" ) 
message( STATUS  ${LEN_WITHOUT_COMMA} )
string(SUBSTRING ${MODULE_COMPONENT_TYPELISTS_} 0 ${LEN_WITHOUT_COMMA} MODULE_COMPONENT_TYPELISTS)
unset(LEN)

configure_file(
  ${${MODULE}_SOURCE_DIR}/include/selxCompiledLibraryComponents.h.in
  ${${MODULE}_BINARY_DIR}/include/selxCompiledLibraryComponents.h
  @ONLY
)
