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
# Private macros

macro( _selxapplication_check_name APPLICATION )
  # "No funny stuff" - Karl Hungus
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid application name: ${APPLICATION}" )
  endif()

  # Check if application was found during initialization
  list( FIND SUPERELASTIX_APPLICATIONS "${APPLICATION}" APPLICATION_FOUND )
  if( ${APPLICATION_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Application not found: ${APPLICATION}" )
  endif()
endmacro()

macro( _selxapplications_initialize )
  if( SUPERELASTIX_APPLICATIONS )
    message( FATAL "Applications have already been initialized. _selxapplications_initialize() may only be called once." )
  endif()

  set( SUPERELASTIX_APPLICATIONS )

  file( GLOB_RECURSE APPLICATION_CMAKE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Applications/*/Application*.cmake"
  )

  message( STATUS "Found the following SuperElastix applications:")
  foreach( APPLICATION_CMAKE_FILE ${APPLICATION_CMAKE_FILES})
    get_filename_component( APPLICATION ${APPLICATION_CMAKE_FILE} NAME_WE )
    get_filename_component( ${APPLICATION}_PATH ${APPLICATION_CMAKE_FILE} PATH )
    
    message( STATUS "  ${APPLICATION}" )

    option( "USE_${APPLICATION}" OFF )
    set( "${APPLICATION}_CMAKE_FILE" ${CMAKE_SOURCE_DIR}/${APPLICATION_CMAKE_FILE} )
    set( "${APPLICATION}_IS_ENABLED" FALSE )

    set( ${APPLICATION}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${${APPLICATION}_PATH} )
    set( ${APPLICATION}_BINARY_DIR ${CMAKE_BINARY_DIR}/${${APPLICATION}_PATH} )
    set( ${APPLICATION}_TEST_DIR ${CMAKE_SOURCE_DIR}/${${APPLICATION}_PATH}/test )

    # Collect header files for Visual Studio Project 
    # http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
    file( GLOB ${APPLICATION}_HEADER_FILES "${${APPLICATION}_SOURCE_DIR}/*/include/*.*")

    list( APPEND SUPERELASTIX_APPLICATIONS ${APPLICATION} )
  endforeach()
endmacro()

macro( _selxapplication_enable APPLICATION UPSTREAM )
  _selxapplication_check_name( ${APPLICATION} )

  message( STATUS "Enabling ${APPLICATION} requested by ${UPSTREAM}.")

  if( NOT ${APPLICATION}_IS_ENABLED )   
    include( ${${APPLICATION}_CMAKE_FILE} )
    set( ${APPLICATION}_IS_ENABLED TRUE ) 
    
    if( NOT ${APPLICATION}_TARGET_NAME )
      set( ${APPLICATION}_TARGET_NAME ${APPLICATION} )
    endif()

    add_executable( ${${APPLICATION}_TARGET_NAME} "${${APPLICATION}_HEADER_FILES}" "${${APPLICATION}_SOURCE_FILES}" )
 
    if( ${APPLICATION}_INCLUDE_DIRS )
      _selxmodule_include_directories( ${${APPLICATION}_TARGET_NAME} ${APPLICATION} )
    endif()

    if( ${APPLICATION}_MODULE_DEPENDENCIES )
      _selxmodule_enable_dependencies( ${APPLICATION}_MODULE_DEPENDENCIES ${APPLICATION} )
      _selxmodule_include_directories( ${${APPLICATION}_TARGET_NAME} ${APPLICATION}_MODULE_DEPENDENCIES )
      _selxmodule_link_libraries( ${${APPLICATION}_TARGET_NAME} ${APPLICATION}_MODULE_DEPENDENCIES )
    endif()

    if( ${APPLICATION}_LINK_LIBRARIES )
      _selxapplication_link_libraries( ${APPLICATION}_TARGET_NAME ${APPLICATION}_LINK_LIBRARIES ) 
    endif()

    message( STATUS "${APPLICATION} enabled." ) 
  else()
    message( STATUS "${APPLICATION} already enabled." )
  endif()
endmacro()

macro( _selxapplication_disable APPLICATION )
  set( USE_${APPLICATION} FALSE )
endmacro()
 
macro( _selxapplication_link_libraries TARGET LIBRARIES )
  target_link_libraries( ${${TARGET}} ${${LIBRARIES}} )
endmacro()

macro( add_integration_test )
  cmake_parse_arguments( add_integration_test "" "NAME;DRIVER" "ARGUMENTS" ${ARGN} )

  add_test(
    NAME ${add_integration_test_NAME}
    COMMAND ${add_integration_test_DRIVER} ${add_integration_test_ARGUMENTS}
  )
endmacro()

# TODO: Allow application developers to use arbitrarty test drivers. We 
# currently CMake for scripting and executable itself to execute tests. 
# Most users will probably prefer not to use CMake for scripting. 

# ---------------------------------------------------------------------
# Public macros

macro( enable_application APPLICATION )
  _selxapplication_enable( ${APPLICATION} "user" )
endmacro()

macro( enable_applications APPLICATIONS )
  foreach( APPLICATION ${${APPLICATIONS}} )
    enable_application( ${APPLICATION} )
  endforeach()
endmacro()

macro( disable_application APPLICATION )
  _selxapplication_disable( ${APPLICATION} "user" )
endmacro()

macro( disable_applications APPLICATIONS )
  foreach( APPLICATION ${${APPLICATIONS}} )
    disable_application( ${APPLICATION} )
  endforeach()
endmacro()
