# Visual Studio complains if paths are too long

if( MSVC )
  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 50 )
  message(
    FATAL_ERROR
      "Source code directory path length is too long for MSVC (${n} > 50)."
      "Please move the source code directory to a directory with a shorter path."
    )
  endif()

  string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
  if( n GREATER 50 )
  message(
    FATAL_ERROR
      "Build directory path length is too long for MSVC (${n} > 50)."
      "Please move the build directory to a directory with a shorter path."
    )
  endif()

  set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
  set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj" )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj" )
endif()