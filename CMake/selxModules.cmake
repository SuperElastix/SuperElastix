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

macro( _selxmodule_check_name MODULE )
  # "No funny stuff" - Karl Hungus
  if( NOT "${_name}" MATCHES "^[a-zA-Z_]*$" )
    message( FATAL_ERROR "Invalid module name: ${MODULE}" )
  endif()

  # Check if module was found during initialization
  list( FIND SUPERELASTIX_MODULES "${MODULE}" MODULE_FOUND )
  if( ${MODULE_FOUND} EQUAL -1 )
    message( FATAL_ERROR "Module not found: ${MODULE}" )
  endif()
endmacro()

macro( _selxmodules_initialize )
  if( SUPERELASTIX_MODULES )
    message( FATAL "Modules have already been initialized. _selxmodules_initialize() may only be called once." )
  endif()

  set( SUPERELASTIX_MODULES )
  set( SUPERELASTIX_INCLUDE_DIRS )
  set( SUPERELASTIX_LIBRARIES )
  set( SUPERELASTIX_TEST_SOURCE_FILES )

  file( GLOB_RECURSE MODULE_CMAKE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/Module*.cmake"
  )

  message( STATUS "Found the following SuperElastix modules:")
  foreach( MODULE_CMAKE_FILE ${MODULE_CMAKE_FILES})
    get_filename_component( MODULE ${MODULE_CMAKE_FILE} NAME_WE )
    get_filename_component( ${MODULE}_PATH ${MODULE_CMAKE_FILE} PATH )
    
    message( STATUS "  ${MODULE}" )

    option( "USE_${MODULE}" OFF )
    set( "${MODULE}_CMAKE_FILE" ${CMAKE_SOURCE_DIR}/${MODULE_CMAKE_FILE} )
    set( "${MODULE}_IS_ENABLED" FALSE )

    set( ${MODULE}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${${MODULE}_PATH} )
    set( ${MODULE}_BINARY_DIR ${CMAKE_BINARY_DIR}/${${MODULE}_PATH} )

    # Collect header files for Visual Studio Project 
    # http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
    file( GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/*/include/*.*")

    set( ${MODULE}_INCLUDE_DIRS )
    set( ${MODULE}_LIBRARY_DIRS )
    set( ${MODULE}_LIBRARIES )

    list( APPEND SUPERELASTIX_MODULES ${MODULE} )
  endforeach()
endmacro()

macro( _selxmodule_enable MODULE UPSTREAM )
  _selxmodule_check_name( ${MODULE} )

  message( STATUS "Enabling ${MODULE} requested by ${UPSTREAM}.")

  if( NOT ${MODULE}_IS_ENABLED )    
    set( ${MODULE}_IS_ENABLED TRUE )
    include( ${${MODULE}_CMAKE_FILE} )
    
    add_library( ${MODULE} "${${MODULE}_HEADER_FILES}" "${${MODULE}_SOURCE_FILES}" )

    if( ${MODULE}_INCLUDE_DIRS )
      _selxmodule_include_directory( ${MODULE} ${MODULE} )
    endif()

    if( NOT ${MODULE} STREQUAL ModuleCore )
      _selxmodule_include_directory( ${MODULE} ModuleCore )
      _selxmodule_link_libraries( ${MODULE} ModuleCore )
    endif()

    if( BUILD_TESTING AND ${MODULE}_TEST_SOURCE_FILES )
      list( APPEND SUPERELASTIX_TEST_SOURCE_FILES ${${MODULE}_TEST_SOURCE_FILES} )
    endif()

    if( ${MODULE}_MODULE_DEPENDENCIES )
      _selxmodule_enable_dependencies( ${MODULE}_MODULE_DEPENDENCIES ${MODULE} )
      _selxmodule_include_directories( ${MODULE} ${MODULE}_MODULE_DEPENDENCIES )
      _selxmodule_link_libraries( ${MODULE} ${MODULE}_MODULE_DEPENDENCIES )

      # TODO: Resolve cylic dependency graph. SuperElastix compiles only because 
      # CMake is not aware of dependencies and because of the specific order in 
      # which modules are compiled
      # add_dependencies( ${MODULE} ${${MODULE}_MODULE_DEPENDENCIES} )
    endif()

    if( ${MODULE}_LINK_LIBARIES )
      _selxmodule_link_libraries( ${MODULE} ${MODULE}_LINK_LIBARIES )
    endif()

    message( STATUS "${MODULE} enabled." ) 

    # SUPERELASTIX_INCLUDE_DIRS and SUPERELASTIX_LIBRARIES are convenience 
    # variables that should only be used when absolutely necessary,  e.g. when
    # compiling tests from a central place in Testing/Unit/CMakeLists.txt
    if( ${MODULE}_INCLUDE_DIRS )
      list( APPEND SUPERELASTIX_INCLUDE_DIRS ${${MODULE}_INCLUDE_DIRS} )
    endif()

    if( ${MODULE}_LIBRARIES )
      list( APPEND SUPERELASTIX_LIBRARIES ${${MODULE}_LIBRARIES} )
    endif()
  else()
    message( STATUS "${MODULE} already enabled." )
  endif()
endmacro()

macro( _selxmodule_enable_dependencies MODULES UPSTREAM )
  foreach( MODULE ${${MODULES}} )
    _selxmodule_enable( ${MODULE} ${UPSTREAM} )
  endforeach()
endmacro()

macro( _selxmodule_disable MODULE )
  set( USE_${MODULE} FALSE )
  list( FILTER SUPERELASTIX_INCLUDE_DIRS MATCHES EXCLUDE> REGEX "(.*)${MODULE}(.*)" )
  list( FILTER SUPERELASTIX_LIBRARY_DIRS MATCHES EXCLUDE> REGEX "(.*)${MODULE}(.*)" )
  list( FILTER SUPERELASTIX_LIBRARIES MATCHES EXCLUDE> REGEX "(.*)${MODULE}(.*)" )
  list( FILTER SUPERELASTIX_TEST_SOURCE_FILES MATCHES EXCLUDE> REGEX "(.*)${MODULE}(.*)" )
endmacro()

macro( _selxmodule_include_directory TARGET DEPENDENCY )
  target_include_directories( ${TARGET} PUBLIC ${${DEPENDENCY}_INCLUDE_DIRS} )
endmacro()

macro( _selxmodule_include_directories TARGET DEPENDENCIES )
  foreach( DEPENDENCY ${${DEPENDENCIES}} )
    _selxmodule_include_directory( ${TARGET} ${DEPENDENCY} )
  endforeach()
endmacro()

macro( _selxmodule_link_libraries TARGET DEPENDENCIES )
  foreach( DEPENDENCY ${${DEPENDENCIES}} )
    target_link_libraries( ${TARGET} ${DEPENDENCY} )
  endforeach()
endmacro()

# ---------------------------------------------------------------------
# Public macros

macro( enable_module MODULE )
  _selxmodule_enable( ${MODULE} "user" )
endmacro()

macro( enable_modules MODULES )
  foreach( MODULE ${${MODULES}} )
    enable_module( ${MODULE} )
  endforeach()
endmacro()

macro( disable_module MODULE )
  _selxmodule_disable( ${MODULE} )
endmacro()

macro( disable_modules MODULES )
  foreach( MODULE ${${MODULES}} )
    disable_module( ${MODULE} "user" )
  endforeach()
endmacro()

# ---------------------------------------------------------------------

# Enable user-selected modules
# TODO: Loop over USE

# Disable user-selected modules
# TODO:



