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

set( ${MODULE}_SOURCE_FILES
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSource.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSink.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSourceFixed.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkImageSourceMoving.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxDisplacementFieldItkImageFilterSink.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkMeshSource.cxx
  ${${MODULE}_SOURCE_DIR}/src/selxItkMeshSink.cxx
)

set( ${MODULE}_LIBRARIES 
  ${MODULE}
)

set( ${MODULE}_TEST_SOURCE_FILES )

set( ${MODULE}_MODULE_DEPENDENCIES 
  ModuleController
)

