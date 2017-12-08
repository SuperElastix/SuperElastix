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
  if( NOT "${MODULE}" MATCHES "^[a-zA-Z0-9]+$" )
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
  set( SUPERELASTIX_LIBRARY_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
  set( SUPERELASTIX_TEST_SOURCE_FILES )
  set( SUPERELASTIX_INTERFACE_DIRS )

  file( GLOB_RECURSE MODULE_CMAKE_FILES RELATIVE "${CMAKE_SOURCE_DIR}"
     "${CMAKE_SOURCE_DIR}/Modules/*/Module*.cmake"
  )
  
  # find the Modules that are ComponentGroups (is subset of MODULE_CMAKE_FILES)
  file( GLOB MODULE_COMPONENT_GROUP_CMAKE_FILES
     "${CMAKE_SOURCE_DIR}/Modules/Components/*/Module*.cmake"
  )
  
  message( STATUS "Found the following SuperElastix modules:")
  foreach( MODULE_CMAKE_FILE ${MODULE_CMAKE_FILES})
    get_filename_component( MODULE ${MODULE_CMAKE_FILE} NAME_WE )
    get_filename_component( ${MODULE}_PATH ${MODULE_CMAKE_FILE} PATH )
    
    message( STATUS "  ${MODULE}" )

    # Any new found module defaults to ON and user-disabled USE_*Modules will be obeyed, since these are cached variable.
    option( USE_${MODULE} "" ON )
    # The actually enabling will be done by when enable_modules is called
    set( ${MODULE}_IS_ENABLED FALSE )

    set( ${MODULE}_CMAKE_FILE ${PROJECT_SOURCE_DIR}/${${MODULE}_PATH}/${MODULE}.cmake )
    set( ${MODULE}_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${${MODULE}_PATH} )
    set( ${MODULE}_BINARY_DIR ${PROJECT_BINARY_DIR}/${${MODULE}_PATH} )

    # Collect header files for Visual Studio 
    # http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
    file( GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/include/*.*" )
    
    # Collect interface files for Visual Studio 
    file( GLOB ${MODULE}_INTERFACE_FILES "${${MODULE}_SOURCE_DIR}/interfaces/*.*" )
    
    # These variables are defined in the module's .cmake file
    set( ${MODULE}_INCLUDE_DIRS )
    set( ${MODULE}_SOURCE_FILES )
    set( ${MODULE}_TEST_SOURCE_FILES )
    set( ${MODULE}_MODULE_DEPENDENCIES )
    set( ${MODULE}_LIBRARIES )

    list( APPEND SUPERELASTIX_MODULES ${MODULE} )

  endforeach()
  
  # Create dummy targets to show header-only Components and Interfaces in the Solution Explorer of Visual Studio
  add_library(HeaderOnlyComponents "${CMAKE_SOURCE_DIR}/Modules/Components/HeaderOnlyComponents.cxx")
  add_library(Interfaces "${CMAKE_SOURCE_DIR}/Modules/Components/Interfaces.cxx")
  
endmacro()

macro( _selxmodule_enable MODULE UPSTREAM )
  _selxmodule_check_name( ${MODULE} )

  message( STATUS "Enabling ${MODULE} requested by ${UPSTREAM}.")

  if( NOT ${MODULE}_IS_ENABLED )    
    set( ${MODULE}_IS_ENABLED TRUE )
    include( ${${MODULE}_CMAKE_FILE} )
    
    if( ${MODULE}_MODULE_DEPENDENCIES )
      _selxmodule_enable_dependencies( ${MODULE} ${MODULE}_MODULE_DEPENDENCIES )
    endif()

    if( ${MODULE}_INCLUDE_DIRS )
      include_directories( ${${MODULE}_INCLUDE_DIRS} )
      list( APPEND SUPERELASTIX_INCLUDE_DIRS ${${MODULE}_INCLUDE_DIRS} )
    endif()

    if( BUILD_TESTING AND ${MODULE}_TEST_SOURCE_FILES )
      list( APPEND SUPERELASTIX_TEST_SOURCE_FILES ${MODULE}_TEST_SOURCE_FILES )
    endif()

    if( ${MODULE}_LIBRARIES )
      list( APPEND SUPERELASTIX_LIBRARIES ${${MODULE}_LIBRARIES} )
    endif()

    # Header-only modules should not be compiled
    if( ${MODULE}_SOURCE_FILES )
      # Check if user accidentally tries to compile header-only library
      if( ${MODULE}_LIBRARIES )
        list( FIND "${MODULE}" ${MODULE}_LIBRARIES _index )
        if( _index GREATER -1 AND NOT ${MODULE}_SOURCE_FILES )
          message( FATAL_ERROR "Compilation of header-only module ${MODULE} requested. Remove ${MODULE} from \$\{MODULE\}_LIBRARIES in ${${MODULE}_CMAKE_FILE}" )
        endif()
      endif()

      add_library( ${MODULE} ${${MODULE}_HEADER_FILES} ${${MODULE}_SOURCE_FILES} )
    else()
    # Aggregate all header-only Components in a separate target for Visual Studio IDE.
      target_sources( HeaderOnlyComponents PUBLIC ${${MODULE}_HEADER_FILES} )
    endif()
	
    # Aggregate all interface headers in a separate target for Visual Studio IDE.
    if( ${MODULE}_INTERFACE_FILES )
        target_sources( Interfaces PUBLIC ${${MODULE}_INTERFACE_FILES} )
    endif()
    if( ${MODULE}_LIBRARY_DIRS )
      list( APPEND SUPERELASTIX_LIBRARY_DIRS ${${MODULE}_LIBRARY_DIRS} )
    endif()
	
    message( STATUS "${MODULE} enabled." ) 
  else()
    message( STATUS "${MODULE} already enabled." )
  endif()
endmacro()

macro( _selxmodule_enable_dependencies UPSTREAM MODULES )
  foreach( MODULE ${${MODULES}} )
    _selxmodule_enable( ${MODULE} ${UPSTREAM} )
  endforeach()
endmacro()

macro( _selxmodule_target_file TARGETS )
  foreach( TARGET ${${TARGETS}} )
    set( ${TARGET}_FILE $<TARGET_FILE:TARGET> )
  endforeach()
endmacro()

# ---------------------------------------------------------------------
# Public macros

macro( enable_module MODULE )
  _selxmodule_enable( ${MODULE} "user" )
endmacro()

# Enable user-selected modules
macro( enable_modules )
  foreach( MODULE ${SUPERELASTIX_MODULES} )
    if( USE_${MODULE} )
      enable_module( ${MODULE} )
    endif()
  endforeach()
  foreach( MODULE ${SUPERELASTIX_MODULES} )
    if( ${${MODULE}_IS_ENABLED})
      set( USE_${MODULE} TRUE CACHE BOOL "" FORCE)
    endif()
  endforeach()  
endmacro()

# ---------------------------------------------------------------------
