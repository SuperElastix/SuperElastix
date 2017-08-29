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

#--------------------------------------------------------------------------
# Plugin API
#--------------------------------------------------------------------------

SET(CONFIG_CREATE_CPPCHECK    0   CACHE BOOL     "add CppCheck target for static code analysis")

##
# \page     ACME_API ACME_API
# \section  ACME_CPPCHECK_TARGET ACME_CPPCHECK_TARGET
# \text  	Add the module specified by the current \a CMakeList.txt to the cppcheck target \n
# \details 	--------------------------------------------------------------------------------------------------------------------\n \n
FUNCTION(ACME_CPPCHECK_TARGET)
	INTERNAL_CPPCHECK_TARGET()
ENDFUNCTION(ACME_CPPCHECK_TARGET)


#--------------------------------------------------------------------------
# Plugin External Tools
#--------------------------------------------------------------------------

MESSAGE(VERBOSE "...requires external tool Cppcheck")
IF(CONFIG_CREATE_CPPCHECK)
	IF(NOT CPPCHECK_FOUND) 
		INCLUDE(${ACME_PATH}/tools/FindCppCheck.cmake)
	ENDIF()
ENDIF()


#--------------------------------------------------------------------------
# Plugin Hooks
#--------------------------------------------------------------------------

IF(CONFIG_CREATE_CPPCHECK)
	ADD_PLUGIN_FINALIZE_HOOK(INTERNAL_CPPCHECK)
ENDIF()


#--------------------------------------------------------------------------
# Plugin Functions
#--------------------------------------------------------------------------

SET(TARGETS_TO_CHECK "" CACHE INTERNAL "")


FUNCTION(INTERNAL_CPPCHECK_TARGET)
	SET(TARGETS_TO_CHECK ${TARGETS_TO_CHECK} ${CURRENT_MODULE_NAME} CACHE INTERNAL "")
ENDFUNCTION(INTERNAL_CPPCHECK_TARGET)


FUNCTION(INTERNAL_CPPCHECK)
	IF(CONFIG_CREATE_CPPCHECK)
		IF(CPPCHECK_FOUND)
			MESSAGE(STATUS "Finalize cppcheck for targets: ${TARGETS_TO_CHECK}")
			add_custom_target(cppcheck COMMENT "static source code analysis with cppcheck")
			add_custom_target(cppcheck_xml COMMENT "static source code analysis with cppcheck (xml export)")

			FOREACH(TARGET_TO_CHECK ${TARGETS_TO_CHECK})
				get_target_property(cppcheckSourceFiles ${TARGET_TO_CHECK} SOURCES)

				# get include dirs of the target
				get_target_property(cppcheckIncludeDirs ${TARGET_TO_CHECK} INCLUDE_DIRECTORIES)
				set( cppcheckIncludes )
				foreach(includeDir ${cppcheckIncludeDirs} )
					LIST (APPEND cppcheckIncludes "-I${includeDir}")
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

				add_custom_command(TARGET cppcheck
					COMMAND ${CPPCHECK_EXECUTABLE}
					ARGS -q --inline-suppr --enable=all ${cppcheckIncludes} ${cppcheckDefines} ${cppcheckSourceFiles}
					)
				add_custom_command(TARGET cppcheck_xml
					COMMAND ${CPPCHECK_EXECUTABLE}
					ARGS --xml-version=2 -q --inline-suppr --enable=all ${cppcheckIncludes} ${cppcheckDefines} ${cppcheckSourceFiles}
					)

			ENDFOREACH()
		ENDIF()
	ENDIF()
ENDFUNCTION(INTERNAL_CPPCHECK)

