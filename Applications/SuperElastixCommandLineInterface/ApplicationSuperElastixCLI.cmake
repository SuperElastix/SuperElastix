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

set( APPLICATION ApplicationSuperElastixCLI )

set( ${APPLICATION}_TARGET_NAME SuperElastix )

set( ${APPLICATION}_SOURCE_FILES
  ${${APPLICATION}_SOURCE_DIR}/src/selxSuperElastix.cxx
)

set( ${APPLICATION}_LINK_LIBRARIES
  ${ITK_LIBRARIES}
  elastix transformix
  ${Boost_LIBRARIES}
)

set( ${APPLICATION}_MODULE_DEPENDENCIES
  ModuleCore
  ModuleController
  ModuleElastix
  ModuleExamples
  ModuleItkImageRegistrationMethodv4
  ModuleItkSmoothingRecursiveGaussianImageFilter
  ModuleSinksAndSources
)

set( ${APPLICATION}_INTEGRATION_TESTS 
  --conf ../Configuration/itkv4_SVF_ANTsCC.json --in FixedImage=../Data/coneA2d64.mhd MovingImage=../Data/coneB2d64.mhd --out ResultImage=2A_image_itkv4_NC.mhd ResultDisplacementField=2A_deformation_itkv4_NC.mhd
)