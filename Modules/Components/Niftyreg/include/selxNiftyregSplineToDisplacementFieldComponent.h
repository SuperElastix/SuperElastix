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

#ifndef selxNiftyregSplineToDisplacementFieldComponent_h
#define selxNiftyregSplineToDisplacementFieldComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"

#include <string.h>
#include <array>

namespace selx
{
template< class TPixel >
class NiftyregSplineToDisplacementFieldComponent :
  public SuperElastixComponent<
  Accepting< NiftyregControlPointPositionImageInterface< TPixel >, NiftyregReferenceImageInterface< TPixel > >,
  Providing< NiftyregDisplacementFieldImageInterface< TPixel >, UpdateInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef NiftyregSplineToDisplacementFieldComponent< TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< NiftyregControlPointPositionImageInterface< TPixel >, NiftyregReferenceImageInterface< TPixel >>,
    Providing< NiftyregDisplacementFieldImageInterface< TPixel >, UpdateInterface >
    >                                      Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  NiftyregSplineToDisplacementFieldComponent( const std::string & name, LoggerImpl & logger );
  virtual ~NiftyregSplineToDisplacementFieldComponent();

  // Accepting NiftyregControlPointPositionImageInterface
  virtual int Accept( typename NiftyregControlPointPositionImageInterface< TPixel >::Pointer ) override;

  // Accepting NiftyregReferenceImageInterface
  virtual int Accept( typename NiftyregReferenceImageInterface< TPixel >::Pointer ) override;

  // Providing NiftyregWarpedImageInterface
  virtual std::shared_ptr< nifti_image > GetDisplacementFieldNiftiImage() override;

  // Providing UpdateInterface
  virtual void Update() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "NiftyregSplineToDisplacementField Component"; }

private:
  typename NiftyregControlPointPositionImageInterface< TPixel >::Pointer m_NiftyregControlPointPositionImageInterface;
  std::shared_ptr< nifti_image > m_reference_image;
  std::shared_ptr< nifti_image > m_cpp_image;
  std::shared_ptr< nifti_image > m_displacement_image;
  

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "NiftyregSplineToDisplacementFieldComponent" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftyregSplineToDisplacementFieldComponent.hxx"
#endif
#endif // #define selxNiftyregSplineToDisplacementFieldComponent_h
