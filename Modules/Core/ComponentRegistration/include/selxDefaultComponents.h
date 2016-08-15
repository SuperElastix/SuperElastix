#include "selxTypeList.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"
#include "selxElastixComponent.h"
#include "selxItkImageSink.h"
#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

namespace selx
{

  using DefaultComponents = selx::TypeList < DisplacementFieldItkImageFilterSinkComponent<2, float>,
    ItkImageSourceFixedComponent<2, float>,
    ItkImageSourceMovingComponent<2, float>,
    ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>,
    ItkImageRegistrationMethodv4Component<2, float>,
    ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<2, float>,
    ItkMeanSquaresImageToImageMetricv4Component<2, float>,
    ElastixComponent<2, float>,
    ItkImageSinkComponent < 2, float >> ;
}
