# Visual Studio complains if paths are too long

string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
if( n GREATER 50 )
message(
  FATAL_ERROR
  "ITK source code directory path length is too long (${n} > 50)."
  "Please move the ITK source code directory to a directory with a shorter path."
  )
endif()

string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
if( n GREATER 50 )
message(
  FATAL_ERROR
  "ITK build directory path length is too long (${n} > 50)."
  "Please set the ITK build directory to a directory with a shorter path."
  )
endif()
