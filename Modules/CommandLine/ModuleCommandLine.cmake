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

set( MODULE ModuleCommandLine )

# Export include files
set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
  )

# Collect header files for Visual Studio Project 
# http://stackoverflow.com/questions/8316104/specify-how-cmake-creates-visual-studio-project
file(GLOB ${MODULE}_HEADER_FILES "${${MODULE}_SOURCE_DIR}/*/include/*.*")

# Export libraries
set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

# Export tests
set( ${MODULE}_TESTS
)

# Module source files
set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/CommandLine.cxx
)

message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF)

set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")

find_package(Boost COMPONENTS program_options system filesystem REQUIRED) 
#find_package(Boost REQUIRED) 

# Compile library
add_library( ${MODULE} STATIC ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES} )
target_link_libraries( ${MODULE} ${SUPERELASTIX_LIBRARIES} ${Boost_LIBRARIES})

# Compile executable
add_executable( SuperElastix ${${MODULE}_SOURCE_FILES} ${${MODULE}_HEADER_FILES} )
target_link_libraries( SuperElastix ${SUPERELASTIX_LIBRARIES} ${Boost_LIBRARIES})
