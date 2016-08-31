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

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"
#include "selxElastixComponent.h"
#include "selxMonolithicElastix.h"
#include "selxMonolithicTransformix.h"
#include "selxItkImageSink.h"
#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkGradientDescentOptimizerv4.h"
#include "selxItkGaussianExponentialDiffeomorphicTransform.h"
#include "selxItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent.h"
#include "selxItkAffineTransform.h"
#include "selxItkTransformDisplacementFilter.h"
#include "selxItkResampleFilter.h"
#include "selxRegistrationController.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

namespace selx
{
using DefaultComponents = selx::TypeList<
  DisplacementFieldItkImageFilterSinkComponent< 2, float >,
  ItkImageSinkComponent< 2, float >,
  ItkImageSourceFixedComponent< 2, float >,
  ItkImageSourceMovingComponent< 2, float >,
  ElastixComponent< 2, float >,
  MonolithicElastixComponent< 2, float >,
  MonolithicTransformixComponent< 2, float >,
  ItkImageRegistrationMethodv4Component< 2, float >,
  ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component< 2, float >,
  ItkMeanSquaresImageToImageMetricv4Component< 2, float >,
  ItkGradientDescentOptimizerv4Component< double >,
  ItkGaussianExponentialDiffeomorphicTransformComponent< double, 2 >,
  ItkGaussianExponentialDiffeomorphicTransformParametersAdaptorsContainerComponent< 2, double >,
  ItkAffineTransformComponent< double, 2 >,
  ItkTransformDisplacementFilterComponent< 2, float, double >,
  ItkResampleFilterComponent< 2, float, double >,
  RegistrationControllerComponent< >,
  ItkSmoothingRecursiveGaussianImageFilterComponent< 2, float >
  >;
}
