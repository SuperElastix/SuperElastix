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
  # "This aggression will not stand man" - The Dude
  if( NOT "${APPLICATION}" MATCHES "^[a-zA-Z0-9]+$" )
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

    option( "USE_${APPLICATION}" ON )
    set( "${APPLICATION}_CMAKE_FILE" ${CMAKE_SOURCE_DIR}/${APPLICATION_CMAKE_FILE} )
    set( "${APPLICATION}_IS_ENABLED" FALSE )

    set( ${APPLICATION}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${${APPLICATION}_PATH} )
    set( ${APPLICATION}_BINARY_DIR ${CMAKE_BINARY_DIR}/${${APPLICATION}_PATH} )

    # These variables are defined in the applications's .cmake file
    set( ${APPLICATION}_INCLUDE_DIRS )
    set( ${APPLICATION}_SOURCE_FILES )
    set( ${APPLICATION}_INTEGRATION_TEST_SOURCE_FILES )
    set( ${APPLICATION}_MODULE_DEPENDENCIES )
    set( ${APPLICATION}_LIBRARY_DIRS )
    set( ${APPLICATION}_LIBRARIES )

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
    set( ${APPLICATION}_IS_ENABLED TRUE )
    include( ${${APPLICATION}_CMAKE_FILE} ) 

    if( ${APPLICATION}_MODULE_DEPENDENCIES )
      # TODO: If we are building as a standalone directory we should just check if enabled instead of enabling
      _selxmodule_enable_dependencies( ${APPLICATION}_MODULE_DEPENDENCIES ${APPLICATION} )
    endif()
    
    if( NOT ${APPLICATION}_TARGET_NAME )
      set( ${APPLICATION}_TARGET_NAME ${APPLICATION} )
    endif()

    if( ${APPLICATION}_INCLUDE_DIRS )
      include_directories( ${${APPLICATION}_INCLUDE_DIRS} )
    endif()

    add_executable( ${${APPLICATION}_TARGET_NAME} "${${APPLICATION}_HEADER_FILES}" "${${APPLICATION}_SOURCE_FILES}" )
    target_link_libraries( ${${APPLICATION}_TARGET_NAME} ${SUPERELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${Boost_LIBRARIES} )

    if( BUILD_TESTING AND ${APPLICATION}_TEST_SOURCE_FILES )
      list( APPEND SUPERELASTIX_TEST_SOURCE_FILES ${APPLICATION}_TEST_SOURCE_FILES )
    endif()

    add_dependencies( ${${APPLICATION}_TARGET_NAME} ModuleCore )

    message( STATUS "${APPLICATION} enabled." ) 
  else()
    message( STATUS "${APPLICATION} already enabled." )
  endif()
endmacro()

macro( _selxapplication_disable APPLICATION )
  set( USE_${APPLICATION} OFF )
endmacro()

macro( add_integration_test )
  cmake_parse_arguments( add_integration_test "" "NAME;DRIVER" "ARGUMENTS" ${ARGN} )

  add_test(
    NAME ${add_integration_test_NAME}
    COMMAND ${add_integration_test_DRIVER} ${add_integration_test_ARGUMENTS}
  )
endmacro()

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
