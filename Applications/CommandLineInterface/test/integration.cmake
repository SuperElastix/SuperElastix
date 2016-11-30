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

add_integration_test(
  NAME SuperElastixIntegrationTest
  DRIVER SuperElastix
  ARGUMENTS --conf ${CMAKE_SOURCE_DIR}/Testing/Data/Configuration/itkv4_SVF_ANTsCC.json --in FixedImage=${CMAKE_BINARY_DIR}/Testing/Data/Input/coneA2d64.mhd MovingImage=${CMAKE_BINARY_DIR}/Testing/Data/Input/coneB2d64.mhd --out ResultImage=2A_image_itkv4_NC.mhd ResultDisplacementField=2A_deformation_itkv4_NC.mhd
)

# Possible grand challenge CMake interface:
# 
# add_grand_challenge_submission( 
#   NAME username
#   DRIVER GrandChallengeDriver
#   ARGUMENTS ...
# )
#  
# The CMake mechanism would be similar to that of integration tests, except
# we would allow users to write their own driver and/or use an executable that 
# contains additional functionality for saving results. The macro would also
# invoke a script that uploads results and configurition files to website.

