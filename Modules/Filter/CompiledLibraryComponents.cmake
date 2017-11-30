#
# This CMakeLists file will generate the file selxDefaultComponents.h
# in the bin directory. This file defines the components to be compiled.
#

set( MODULE_COMPONENT_GROUP_INCLUDE_FILES "#include \"selxModuleSinksAndSources.h\"" )
set( MODULE_COMPONENT_TYPELISTS "ModuleSinksAndSourcesComponents," )

configure_file(
  ${${MODULE}_SOURCE_DIR}/include/selxCompiledLibraryComponents.h.in
  ${${MODULE}_BINARY_DIR}/include/selxCompiledLibraryComponents.h
  @ONLY
)
