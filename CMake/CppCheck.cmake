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

# Parts taken from code from ACME https://github.com/bmwcarit/acme
# which has the following copyright notice:
#
# Copyright (C) 2012 BMW Car IT GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

SET(CONFIG_CREATE_CPPCHECK    1   CACHE BOOL     "add CppCheck target for static code analysis")


IF(CONFIG_CREATE_CPPCHECK)
	find_program(CPPCHECK_EXECUTABLE
	  cppcheck
	  PATHS
	  /usr/bin
	  "$ENV{ProgramFiles}/Cppcheck"
	  "C:/Program Files (x86)/Cppcheck"
	  "C:/Program Files/Cppcheck"
	)
ENDIF()


FUNCTION(INTERNAL_CPPCHECK_TARGET)
	SET(TARGETS_TO_CHECK ${TARGETS_TO_CHECK} ${CURRENT_MODULE_NAME} CACHE INTERNAL "")
ENDFUNCTION(INTERNAL_CPPCHECK_TARGET)


FUNCTION(INTERNAL_CPPCHECK)
	IF(CONFIG_CREATE_CPPCHECK)
		IF(CPPCHECK_EXECUTABLE)
			MESSAGE(STATUS "Finalize cppcheck for targets: ${TARGETS_TO_CHECK}")
			add_custom_target(cppcheck COMMENT "static source code analysis with cppcheck")

			FOREACH(TARGET_TO_CHECK ${TARGETS_TO_CHECK})
				get_target_property(cppcheckSourceFiles ${TARGET_TO_CHECK} SOURCES)

				# get include dirs of the target
				get_target_property(cppcheckIncludeDirs ${TARGET_TO_CHECK} INCLUDE_DIRECTORIES)
				
				# Use a file with all the includes instead of putting them on the commandline
				SET(cppcheck_include_directories_file "${CMAKE_CURRENT_BINARY_DIR}/cppcheck_includes.txt")
				FILE(WRITE ${cppcheck_include_directories_file} "")
				foreach(include_dir ${cppcheckIncludeDirs})
					FILE(APPEND ${cppcheck_include_directories_file} "${include_dir}" \n)
				endforeach()

				# get compile definitions
				
				IF(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
					STRING(TOUPPER ${CMAKE_BUILD_TYPE} buildTypeUpper)
				ENDIF()
				IF ("${buildTypeUpper}" STREQUAL "")
					SET(buildTypeUpper "DEBUG")
				ENDIF()
				get_target_property(cppcheckDefinesRaw ${TARGET_TO_CHECK} COMPILE_DEFINITIONS_${buildTypeUpper})
				set( cppcheckDefines )
				foreach(cppcheckDefine ${cppcheckDefinesRaw} )
					LIST (APPEND cppcheckDefines "-D${cppcheckDefine}")
				endforeach()

				add_custom_command(TARGET cppcheck COMMAND ${CPPCHECK_EXECUTABLE} ARGS
					-q
					--inline-suppr
					--enable=all 
					"--includes-file=${cppcheck_include_directories_file}"
					${cppcheckDefines}
					${cppcheckSourceFiles}
				)

			ENDFOREACH()
		ENDIF()
	ENDIF()
ENDFUNCTION(INTERNAL_CPPCHECK)

