#=========================================================================
#
#  Copyright Leiden University Medical Center, Erasmus University Medical 
#  Center and contributors
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#=========================================================================

# ---------------------------------------------------------------------
# Private helper macros

macro( _selxmodule_check_name MODULE )
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid module name: ${MODULE}" )
  endif()

  list( FIND SUPERELASTIX_MODULES "${MODULE}" MODULE_FOUND )
  if( ${MODULE_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Module not found: ${MODULE}")
  endif()
endmacro()

macro( _selxmodule_enable MODULE_NAME )
  _selxmodule_check_name( ${MODULE_NAME} )

  if( NOT ${MODULE_NAME}_ENABLED )
    set( ${MODULE_NAME}_ENABLED ON )

    include( ${${MODULE_NAME}_FILE} )

    if( ${MODULE_NAME}_INCLUDE_DIRS )
      include_directories( ${${MODULE_NAME}_INCLUDE_DIRS} )
      list( APPEND SUPERELASTIX_INCLUDE_DIRS ${${MODULE_NAME}_INCLUDE_DIRS} )
    endif()

    if( ${MODULE_NAME}_LIBRARIES )
      link_directories( ${${MODULE_NAME}_LIBRARY_DIRS} )
      list( APPEND SUPERELASTIX_LIBRARIES ${${MODULE_NAME}_LIBRARIES} )
    endif()

    if( ${MODULE_NAME}_TESTS )
      list( APPEND SUPERELASTIX_TESTS ${${MODULE_NAME}_TESTS} )
    endif()
    
    # TODO: Add support for indicating dependencies between modules and recursive enabling of these dependencies

  endif()
endmacro()

macro( _selxmodule_disable MODULE_NAME )
  # TODO: selxmodule_disable
endmacro()

macro( _selxmodules_initialize )
  set( SUPERELASTIX_MODULES )
  set( SUPERELASTIX_INCLUDE_DIRS )
  set( SUPERELASTIX_LIBRARIES )
  set( SUPERELASTIX_TESTS )

  file( GLOB_RECURSE MODULE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/Module*.cmake"
  )

  message( STATUS "Found the following SuperElastix modules:")
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

    list( APPEND SUPERELASTIX_MODULES ${MODULE_NAME} )
  endforeach()
endmacro()

_selxmodules_initialize()

# ---------------------------------------------------------------------
# Public interface

macro( selxmodule_enable MODULE )
  _selxmodule_enable( ${MODULE} )
endmacro()

