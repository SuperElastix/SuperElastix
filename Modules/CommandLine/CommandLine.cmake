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

set( COMMANDLINE_SOURCE_DIR
  ${CMAKE_SOURCE_DIR}/Modules/CommandLine
  )

  
# Export include files
set( COMMANDLINE_INCLUDE_DIRS
  ${COMMANDLINE_SOURCE_DIR}/include
  )

# Collect header files for Visual Studio Project 
# http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
file(GLOB COMMANDLINE_HEADER_FILES "${COMMANDLINE_SOURCE_DIR}/*/include/*.*")

# Module source files
set( COMMANDLINE_SOURCE_FILES
  ${COMMANDLINE_SOURCE_DIR}/src/selxCommandLine.cxx
)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF)

set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED) 

# Compile executable
add_executable( SuperElastix ${COMMANDLINE_SOURCE_FILES} ${COMMANDLINE_HEADER_FILES} )
target_link_libraries( SuperElastix ${SUPERELASTIX_LIBRARIES} ${Boost_LIBRARIES})
