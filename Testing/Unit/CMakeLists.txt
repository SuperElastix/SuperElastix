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
if( SUPERELASTIX_BUILD_LONG_TESTS )
  add_definitions( -DSUPERELASTIX_BUILD_LONG_TESTS )
endif()

# ---------------------------------------------------------------------
# Build tests

foreach( SuperElastixTestFileName ${SUPERELASTIX_TEST_SOURCE_FILES} )
  # Build tests executables
  get_filename_component( SuperElastixTest ${SuperElastixTestFileName} NAME_WE )
  add_executable( ${SuperElastixTest} ${SuperElastixTestFileName} )
  target_include_directories( ${SuperElastixTest} PUBLIC {SUPERELASTIX_INCLUDE_DIRS} )
  target_link_libraries( "${SuperElastixTest}" ${SUPERELASTIX_LIBRARIES} ${ITK_LIBRARIES} ${TEST_LIBRARIES} )

  # Add GoogleTest to CTest
  GTEST_ADD_TESTS( ${SuperElastixTest} "--gtest_output=xml:${CMAKE_BINARY_DIR}/Testing/Unit/${SuperElastixTest}.xml" ${SuperElastixTestFileName} )
endforeach()
