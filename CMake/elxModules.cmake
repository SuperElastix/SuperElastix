# ---------------------------------------------------------------------
# Private helper macros

macro( _elxmodule_check_name MODULE )
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid module name: ${MODULE}" )
  endif()

  list( FIND ELASTIX_MODULES "${MODULE}" MODULE_FOUND )
  if( ${MODULE_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Module not found: ${MODULE}")
  endif()
endmacro()

macro( _elxmodule_enable MODULE )
  _elxmodule_check_name( ${MODULE} )

  if( NOT ${MODULE}_ENABLED )
    set( ELASTIX_USE_${MODULE} ON )

    include( ${${MODULE}_FILE} )

    if( ${MODULE}_INCLUDE_DIRS )
      include_directories( ${${MODULE}_INCLUDE_DIRS} )
    endif()

    if( ${MODULE}_LIBRARIES )
      link_directories( ${${MODULE}_LIBRARY_DIRS} )
      list( APPEND ELASTIX_LIBRARIES ${${MODULE}_LIBRARIES} )
    endif()

    # TODO: Add support for indicating dependencies between modules and recursive enabling of these dependencies

  endif()
endmacro()

macro( _elxmodule_disable MODULE )
  # TODO
endmacro()

macro( _elxmodules_initialize )
  set( ELASTIX_LIBRARIES )
  set( ELASTIX_MODULES )

  file( GLOB_RECURSE MODULE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/Module*.cmake"
  )

  message( STATUS "Found the following elastix modules:")
  foreach( MODULE_FILE ${MODULE_FILES})
    get_filename_component( MODULE_NAME ${MODULE_FILE} NAME_WE )
    get_filename_component( ${MODULE_NAME}_PATH ${MODULE_FILE} PATH )
    
    message( STATUS "  ${MODULE_NAME}" )

    option( "ELASTIX_USE_${MODULE_NAME}" OFF )
    set( "${MODULE_NAME}_FILE" ${CMAKE_SOURCE_DIR}/${MODULE_FILE} )
    set( "${MODULE_NAME}_ENABLED" OFF )

    set( ${MODULE_NAME}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${${MODULE_NAME}_PATH} )
    set( ${MODULE_NAME}_BINARY_DIR ${CMAKE_BINARY_DIR}/${${MODULE_NAME}_PATH} )

    set( ${MODULE_NAME}_INCLUDE_DIRS )
    set( ${MODULE_NAME}_LIBRARY_DIRS )
    set( ${MODULE_NAME}_LIBRARIES )

    list(APPEND ELASTIX_MODULES ${MODULE_NAME} )
  endforeach()
endmacro()

_elxmodules_initialize()

# ---------------------------------------------------------------------
# Public interface

macro( elxmodule_enable MODULE )
  _elxmodule_enable( ${MODULE} )
endmacro()

