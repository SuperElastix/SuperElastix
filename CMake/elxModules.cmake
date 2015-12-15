# ---------------------------------------------------------------------
# Private helper macros

macro( _elxmodule_check_name MODULE )
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid module name: ${MODULE}" )
  endif()

  list( FIND SUPERELASTIX_MODULES "${MODULE}" MODULE_FOUND )
  if( ${MODULE_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Module not found: ${MODULE}")
  endif()
endmacro()

macro( _elxmodule_enable MODULE_NAME )
  _elxmodule_check_name( ${MODULE_NAME} )

  if( NOT ${MODULE_NAME}_ENABLED )
    set( USE_${MODULE_NAME} ON )

    include( ${${MODULE_NAME}_FILE} )

    if( ${MODULE_NAME}_INCLUDE_DIRS )
      include_directories( ${${MODULE_NAME}_INCLUDE_DIRS} )
    endif()

    if( ${MODULE_NAME}_LIBRARIES )
      link_directories( ${${MODULE_NAME}_LIBRARY_DIRS} )
      list( APPEND SUPERELASTIX_LIBRARIES ${${MODULE_NAME}_LIBRARIES} )
    endif()

    # TODO: Add support for indicating dependencies between modules and recursive enabling of these dependencies

  endif()
endmacro()

macro( _elxmodule_disable MODULE_NAME )
  # TODO: elxmodule_disable
endmacro()

macro( _elxmodules_initialize )
  set( SUPERELASTIX_LIBRARIES )
  set( SUPERELASTIX_MODULES )

  file( GLOB_RECURSE MODULE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/Module*.cmake"
  )

  message( STATUS "Found the following elastix modules:")
  foreach( MODULE_FILE ${MODULE_FILES})
    get_filename_component( MODULE_NAME ${MODULE_FILE} NAME_WE )
    get_filename_component( ${MODULE_NAME}_PATH ${MODULE_FILE} PATH )
    
    message( STATUS "  ${MODULE_NAME}" )

    option( "USE_${MODULE_NAME}" OFF )
    set( "${MODULE_NAME}_FILE" ${CMAKE_SOURCE_DIR}/${MODULE_FILE} )
    set( "${MODULE_NAME}_ENABLED" OFF )

    set( ${MODULE_NAME}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${${MODULE_NAME}_PATH} )
    set( ${MODULE_NAME}_BINARY_DIR ${CMAKE_BINARY_DIR}/${${MODULE_NAME}_PATH} )

    set( ${MODULE_NAME}_INCLUDE_DIRS )
    set( ${MODULE_NAME}_LIBRARY_DIRS )
    set( ${MODULE_NAME}_LIBRARIES )

    list(APPEND SUPERELASTIX_MODULES ${MODULE_NAME} )
  endforeach()
endmacro()

_elxmodules_initialize()

# ---------------------------------------------------------------------
# Public interface

macro( elxmodule_enable MODULE )
  _elxmodule_enable( ${MODULE} )
endmacro()

