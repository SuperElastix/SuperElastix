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
# To add a test to the build system, append it to the module's list of
# tests in the module's CMake file (e.g. "Modules/Core/ModuleCore.cmake"). 
# Any GoogleTests in these files are automatically added to CTest and
# the SuperElastix dashboard. 

# ---------------------------------------------------------------------
# Options
if( BUILD_LONG_UNIT_TESTS )
  add_definitions( -DSUPERELASTIX_BUILD_LONG_UNIT_TESTS )
endif()

# ---------------------------------------------------------------------
# Build tests

message( STATUS "Enabling tests ..." )
foreach( ModuleTestSourceFileNames ${SUPERELASTIX_TEST_SOURCE_FILES} )
  # Get module name
  string( FIND ${ModuleTestSourceFileNames} "_" FIRST_UNDERSCORE_POS )
  string( SUBSTRING ${ModuleTestSourceFileNames} 0 ${FIRST_UNDERSCORE_POS} ModuleName )

  # Build module test driver
  add_executable( ${ModuleName}TestDriver ${${ModuleTestSourceFileNames}} )
  target_include_directories( ${ModuleName}TestDriver PUBLIC ${SUPERELASTIX_INCLUDE_DIRS} )
  target_link_libraries( ${ModuleName}TestDriver ${SUPERELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  # Add GoogleTest to CTest
  add_test(NAME ${ModuleName}Test COMMAND ${ModuleName}TestDriver "--gtest_output=xml:${CMAKE_BINARY_DIR}/Testing/Unit/${ModuleName}.xml")
  message( STATUS "${ModuleName} test enabled." )
endforeach()
message( STATUS "Enabling tests ... Done" )
