/*=========================================================================
*
*  Copyright Leiden University Medical Center, Erasmus University Medical
*  Center and contributors
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*        http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/

#include "selxTypeList.h"


//Component group ItkSmoothingRecursiveGaussianImageFilter
#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"

//Component group SinksAndSources
#include "selxItkImageSourceComponent.h"
#include "selxItkImageSinkComponent.h"
#include "selxItkDisplacementFieldSourceComponent.h"
#include "selxItkDisplacementFieldSinkComponent.h"

//Component group Elastix
#include "selxMonolithicElastixComponent.h"
#include "selxMonolithicTransformixComponent.h"

//Component group ItkImageRegistrationMethodv4
#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4Component.h"
#include "selxItkMeanSquaresImageToImageMetricv4Component.h"
#include "selxItkGradientDescentOptimizerv4Component.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformComponent.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.h"
#include "selxItkAffineTransformComponent.h"
#include "selxItkTransformDisplacementFilterComponent.h"
#include "selxItkResampleFilterComponent.h"
#include "selxItkTransformSourceComponent.h"
#include "selxItkTransformSinkComponent.h"

//Component group Niftyreg
#include "selxItkToNiftiImageSourceComponent.h"
#include "selxNiftiToItkImageSinkComponent.h"
#include "selxNiftyregf3dComponent.h"
#include "selxNiftyregSplineToDisplacementFieldComponent.h"
#include "selxDisplacementFieldNiftiToItkImageSinkComponent.h"
#include "selxNiftyregAladinComponent.h"

//Component group Examples
#include "selxIdentityTransformRegistrationComponent.h"


namespace selx
{
using DefaultComponents = selx::TypeList<
  ItkDisplacementFieldSinkComponent< 2, float >,
  ItkDisplacementFieldSinkComponent< 3, float >,
  ItkImageSinkComponent< 2, float >,
  ItkImageSinkComponent< 3, short >,
  ItkImageSourceComponent< 2, float >,
  ItkImageSourceComponent< 3, short >,
  ItkImageSourceComponent< 3, unsigned char >,
  MonolithicElastixComponent< 2, float >,
  MonolithicElastixComponent< 3, short >,
  MonolithicTransformixComponent< 2, float >,
  ItkImageRegistrationMethodv4Component< 2, float, double >,
  ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
  ItkMeanSquaresImageToImageMetricv4Component< 2, float, double >,
  ItkGradientDescentOptimizerv4Component< double >,
  ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
  ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
  ItkAffineTransformComponent< double, 2 >,
  ItkTransformDisplacementFilterComponent< 2, float, double >,
  ItkResampleFilterComponent< 2, float, double >,
  ItkSmoothingRecursiveGaussianImageFilterComponent< 2, float >,
  ItkTransformSourceComponent< 2, double >,
  ItkTransformSinkComponent< 2, double >,
  ItkToNiftiImageSourceComponent< 2, float >,
  ItkToNiftiImageSourceComponent< 3, float >,
  NiftiToItkImageSinkComponent< 2, float >,
  NiftiToItkImageSinkComponent< 3, float >,
  NiftyregSplineToDisplacementFieldComponent< float>,
  DisplacementFieldNiftiToItkImageSinkComponent< 2, float>,
  DisplacementFieldNiftiToItkImageSinkComponent< 3, float>,
  NiftyregAladinComponent< float >,
  IdentityTransformRegistrationComponent< 2, float>
  >;
}
