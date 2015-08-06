# GoogleTest needs static linking
include( ${CMAKE_SOURCE_DIR}/CMake/elxCompilerFlags.cmake )
foreach( CompilerFlag ${CompilerFlags} )
  string( REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}" )
endforeach()
