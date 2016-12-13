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

set( ${MODULE}_INCLUDE_DIRS
  ${${MODULE}_SOURCE_DIR}/include
)

# This module is header-only and does not contain any source files
set( ${MODULE}_SOURCE_FILES
)

# This module is header-only and does not export any libraries
set( ${MODULE}_LIBRARIES 
)

set( ${MODULE}_TEST_SOURCE_FILES
  selxitkImageFilterTest.cxx
)

