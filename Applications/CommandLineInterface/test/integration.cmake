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

#Application Demo 1A: should match <source>/Applications/CommandLineInterface/Demo/Scripts_[Linux|Windows]/
add_test(NAME Integration_Demo_1A COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1A.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/elastix_Bspline_NC.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1A.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1A_image_elastix_NC.mhd)

#Application Demo 1B: should match <source>/Applications/CommandLineInterface/Demo/Scripts_[Linux|Windows]/
add_test(NAME Integration_Demo_1B COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1B.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/elastix_Bspline_MSD.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1B.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_1B_image_elastix_MSD.mhd)

#Application Demo 2A: should match <source>/Applications/CommandLineInterface/Demo/Scripts_[Linux|Windows]/
add_test(NAME Integration_Demo_2A COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2A.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itkv4_SVF_ANTsCC.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2A.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2A_image_itkv4_NC.mhd
        ResultDisplacementField=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2A_deformation_itkv4_NC.mhd)

#Application Demo 2B: should match <source>/Applications/CommandLineInterface/Demo/Scripts_[Linux|Windows]/
add_test(NAME Integration_Demo_2B COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2B.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itkv4_SVF_MSD.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2B.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2B_image_itkv4_MSD.mhd
        ResultDisplacementField=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_Demo_2B_deformation_itkv4_MSD.mhd)

add_test(NAME Integration_WarpByItkTransform COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarpByItkTransform.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itk_warper.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarpByItkTransform.dot 
  --in FixedAndMovingImageSource=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       TransformSource=${SUPERELASTIX_INPUT_DATA_DIR}/ItkAffine2Dtransform.tfm 
  --out ResultImageSink=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_WarpByItkTransform.mhd)

add_test(NAME Integration_ComposeBlueprintElastix COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintElastix.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/elastix_Base.json
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/elastix_Blueprint_Bspline_MSD.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintElastix.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd 
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintElastix.mhd)
  
add_test(NAME Integration_ComposeBlueprintItk COMMAND SuperElastix
  --logfile ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintElastix.log 
  --loglevel trace
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itkv4_Base.json
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itkv4_Affine_MSD.json
  --conf ${SUPERELASTIX_CONFIGURATION_DATA_DIR}/itk_TransformSink.json
  --graphout ${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintItk.dot 
  --in FixedImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneA2d64.mhd 
       MovingImage=${SUPERELASTIX_INPUT_DATA_DIR}/coneB2d64.mhd 
  --out ResultImage=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintItk.mhd
        ResultDisplacementField=${SUPERELASTIX_OUTPUT_DATA_DIR}/Integration_ComposeBlueprintItk_def.mhd
        ResultTransform=Integration_ComposeBlueprintItk.tfm)
  
  