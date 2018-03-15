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

# Integration tests are "short" tests that typically test a combination of components by using the SuperElastix commandline interface with a configuration file.

add_test(NAME Integration_WarperItkTransform COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarperItkTransform.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itk_warper.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarperItkTransform.dot 
  --in FixedAndMovingImageSource=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       TransformSource=${SUPERELASTIX_INPUT_DATA_DIR}/ItkAffine2Dtransform.tfm 
  --out ResultImageSink=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarperItkTransform.mhd)
