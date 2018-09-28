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

# This file allows the SuperElastix command line interface to be
# built as an external project. 

# ---------------------------------------------------------------------

set( COMMANDLINE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/selxSuperElastix.cxx
)

#set(Boost_DEBUG ON )
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost COMPONENTS program_options filesystem system regex REQUIRED QUIET ) 
include_directories( ${Boost_INCLUDE_DIR} )

# Compile executable
include_directories(  
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_BINARY_DIR}/Applications"  # For selxGitRevisionSha.h
)
add_executable( SuperElastix ${COMMANDLINE_SOURCE_FILES} ${COMMANDLINE_HEADER_FILES} )
target_link_libraries( SuperElastix ${SUPERELASTIX_LIBRARIES} ${Boost_LIBRARIES} ${ITK_LIBRARIES} ${ELASTIX_LIBRARIES} )

# demo copies SuperElastix executable, image data, configuration files and bat/bash scripts to the DEMO_PREFIX directory
set( DEMO_PREFIX ${PROJECT_BINARY_DIR}/Demo CACHE PATH "Demo files will be copied to this directory" )

# Download demo data
include( ExternalData )

# Get the demo image data: from superelastix-build or otherwise download 
list( APPEND ExternalData_URL_TEMPLATES
  "file://${PROJECT_BINARY_DIR}/../SuperElastix-build/ExternalData/Objects/%(algo)/%(hash)"
  "http://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
  "https://midas3.kitware.com/midas/api/rest?method=midas.bitstream.download&checksum=%(hash)&algorithm=%(algo)"
)

file( GLOB_RECURSE DemoDataContentLinks "*.md5" )
foreach( DemoDataContentLink ${DemoDataContentLinks} )
  string( REGEX REPLACE "\\.md5$" "" DemoDataContentLink ${DemoDataContentLink} )
  ExternalData_Expand_Arguments( demo
    demoFileName
    DATA{${DemoDataContentLink}}
  )
  list( APPEND demoFileNames ${demoFileName} )
endforeach()

# Create target and reference the external data
ExternalData_Add_Target( demo )
add_dependencies( demo SuperElastix )
set_target_properties( demo PROPERTIES EXCLUDE_FROM_ALL TRUE ) 

foreach(demoFileName ${demoFileNames})
  add_custom_command(
    TARGET demo POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${DEMO_PREFIX}/Data
    COMMAND ${CMAKE_COMMAND} -E copy ${demoFileName} ${DEMO_PREFIX}/Data/
  )
endforeach()

# Copy executable
add_custom_command(
  TARGET demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SuperElastix> ${DEMO_PREFIX}/$<TARGET_FILE_NAME:SuperElastix>
)

# Copy Configuration files
set( ConfigFiles 
  "itkv4_SVF_ANTsCC.json" 
  "itkv4_SVF_MSD.json"
  "elastix_Bspline_NC.json"
  "elastix_Bspline_MSD.json"
  "IdentityTransformRegistration.json"
  "elastix3d.json"
)

foreach( ConfigFile ${ConfigFiles} )
  add_custom_command(
    TARGET demo POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${DEMO_PREFIX}/Configuration
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../Testing/Data/Configuration/${ConfigFile}" ${DEMO_PREFIX}/Configuration/${ConfigFile}
  )
endforeach()

# Copy bat/bash scripts
set( ScriptFiles 
  "1A_SuperElastix_elastix_NC" 
  "1B_SuperElastix_elastix_MSD"
  "2A_SuperElastix_itkv4_NC"
  "2B_SuperElastix_itkv4_MSD"
  "IdentityTransformRegistration"
  "elastix3d"
  "graphviz_to_png"
)

if(WIN32)
  foreach( ScriptFile ${ScriptFiles} )
    add_custom_command(
      TARGET demo POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DEMO_PREFIX}/Scripts
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Demo/Scripts_Windows/${ScriptFile}.bat" "${DEMO_PREFIX}/Scripts/${ScriptFile}.bat")
  endforeach()
else(WIN32)
  foreach( ScriptFile ${ScriptFiles} )
    add_custom_command(
      TARGET demo POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DEMO_PREFIX}/Scripts
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Demo/Scripts_Linux/${ScriptFile}.sh" "${DEMO_PREFIX}/Scripts/${ScriptFile}.sh")
  endforeach()
endif(WIN32)
