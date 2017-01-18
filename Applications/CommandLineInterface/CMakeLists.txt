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

# Collect header files for Visual Studio Project 
# http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
file(GLOB COMMANDLINE_HEADER_FILES "${COMMANDLINE_SOURCE_DIR}/*/include/*.*")

set( COMMANDLINE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/selxSuperElastix.cxx
)

#set(Boost_DEBUG ON )
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME    OFF)

set(BOOST_ROOT ${BOOST_ROOT}/BOOST-prefix/src/Boost )
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/boost )
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)

find_package(Boost COMPONENTS program_options system filesystem REQUIRED QUIET ) 
include_directories(${Boost_INCLUDE_DIR})

# Compile executable
add_executable( SuperElastix ${COMMANDLINE_SOURCE_FILES} ${COMMANDLINE_HEADER_FILES} )
target_link_libraries( SuperElastix ${SUPERELASTIX_LIBRARIES} ${Boost_LIBRARIES})

# InstallDemo copies SuperElastix executable, image data, configuration files and bat/bash scripts to the INSTALLDEMO_PREFIX directory
set( INSTALLDEMO_PREFIX ${PROJECT_BINARY_DIR}/Demo CACHE PATH "Demo files will be copied to this directory" )

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
  ExternalData_Expand_Arguments( InstallDemo
    InstallDemoFileName
    DATA{${DemoDataContentLink}}
  )
  list( APPEND InstallDemoFileNames ${InstallDemoFileName} )
endforeach()

# Create target and reference the external data
ExternalData_Add_Target( InstallDemo )
add_dependencies( InstallDemo SuperElastix )
set_target_properties( InstallDemo PROPERTIES EXCLUDE_FROM_ALL TRUE ) 

foreach(InstallDemoFileName ${InstallDemoFileNames})
  add_custom_command(
    TARGET InstallDemo POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALLDEMO_PREFIX}/Data
    COMMAND ${CMAKE_COMMAND} -E copy ${InstallDemoFileName} ${INSTALLDEMO_PREFIX}/Data/
  )
endforeach()

# Copy executable
add_custom_command(
  TARGET InstallDemo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SuperElastix> ${INSTALLDEMO_PREFIX}/$<TARGET_FILE_NAME:SuperElastix>
)

# Copy Configuration files
set( ConfigFiles 
  "itkv4_SVF_ANTsCC.json" 
  "itkv4_SVF_MSD.json"
  "elastix_Bspline_NC.json"
  "elastix_Bspline_MSD.json"
)

foreach( ConfigFile ${ConfigFiles} )
  add_custom_command(
    TARGET InstallDemo POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALLDEMO_PREFIX}/Configuration
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/../Testing/Data/Configuration/${ConfigFile}" ${INSTALLDEMO_PREFIX}/Configuration/${ConfigFile}
  )
endforeach()

# Copy bat/bash scripts
set( ScriptFiles 
  "1A_SuperElastix_elastix_NC" 
  "1B_SuperElastix_elastix_MSD"
  "2A_SuperElastix_itkv4_NC"
  "2B_SuperElastix_itkv4_MSD"
)

if(WIN32)
  foreach( ScriptFile ${ScriptFiles} )
    add_custom_command(
      TARGET InstallDemo POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALLDEMO_PREFIX}/Scripts
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Demo/Scripts_Windows/${ScriptFile}.bat" "${INSTALLDEMO_PREFIX}/Scripts/${ScriptFile}.bat")
  endforeach()
else( WIN32 )
  foreach( ScriptFile ${ScriptFiles} )
    add_custom_command(
      TARGET InstallDemo PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALLDEMO_PREFIX}/Scripts
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Demo/Scripts_Linux/${ScriptFile}.sh" "${INSTALLDEMO_PREFIX}/Scripts/${ScriptFile}.sh")
  endforeach()
endif(WIN32)
