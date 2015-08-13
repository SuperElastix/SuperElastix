# GoogleTest needs static linking

include( elxCompilerFlags.cmake )
foreach( CompilerFlag ${CompilerFlags} )
  string( REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}" )
endforeach()
