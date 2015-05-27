include( ${CMAKE_SOURCE_DIR}/CMake/elxCompilerFlags.cmake )

foreach( CompilerFlag ${CompilerFlags} )
  # GoogleTest needs static linking
  string( REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}" )
endforeach()
