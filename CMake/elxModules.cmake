# ---------------------------------------------------------------------
# Private helper macros

macro( _elxmodule_check_name MODULE )
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid module name: ${MODULE}" )
  endif()

  list( FIND ELXMODULE_ALL "${MODULE}" MODULE_FOUND )
  if( ${MODULE_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Module not found: ${MODULE}")
  endif()
endmacro()

macro( _elxmodule_enable MODULE )
  _elxmodule_check_name( ${MODULE} )

  if( NOT ${MODULE}_ENABLED )
    set( "${MODULE}_ENABLED" ON )

    add_subdirectory( "${${MODULE}_SOURCE_DIR}" )

    if( ${MODULE}_INCLUDE_DIRS )
      include_directories( ${${MODULE}_INCLUDE_DIRS} )
    endif()

    if( ${MODULE}_LIBRARIES )
      link_directories( ${${MODULE}_LIBRARY_DIR} )
    endif()

    if( ${${MODULE}_LIBRARIES} )
      list( APPEND ELASTIX_LIBRARIES
        ${${MODULE}_LIBRARIES}
      )
    endif()

    # TODO: Add recursive dependency walk
    # foreach( DEPENDENCY IN LISTS ${MODULE}_DEPENDS )
    #   _elxmodule_enable( ${DEPENDENCY} )
    # endforeach()
  endif()
endmacro()

macro( _elxmodule_disable MODULE )
  # TODO
endmacro()

macro( _elxmodules_initialize )
  set( ELXMODULE_ALL )

  file( GLOB MODULE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/ELXMODULE_*.cmake"
  )

  message( STATUS "Found the following elastix modules:")
  foreach( MODULE_FILE ${MODULE_FILES})
    get_filename_component( MODULE_PATH ${MODULE_FILE} PATH )
    get_filename_component( MODULE ${MODULE_FILE} NAME_WE )
    message( STATUS "  ${MODULE}" )

    option( "USE_${MODULE}" OFF )
    set( "${MODULE}_ENABLED" OFF )

    set( ${MODULE}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${MODULE_PATH} )
    set( ${MODULE}_BINARY_DIR ${CMAKE_BINARY_DIR}/${MODULE_PATH} )
    set( ${MODULE}_LIBRARIES )

    list(APPEND ELXMODULE_ALL ${MODULE} )
  endforeach()
endmacro()

_elxmodules_initialize()

# ---------------------------------------------------------------------
# Public interface

macro( elxmodule_enable MODULE )
  option( USE_${MODULE} ON )
  _elxmodule_enable( ${MODULE} )
endmacro()


