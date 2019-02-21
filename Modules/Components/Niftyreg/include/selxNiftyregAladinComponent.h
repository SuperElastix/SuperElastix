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

#ifndef selxNiftyregAladinComponent_h
#define selxNiftyregAladinComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include "_reg_aladin.h"

#include <string.h>
#include <array>

namespace selx
{
template< class TPixel >
class NiftyregAladinComponent :
  public SuperElastixComponent<
  Accepting< NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
             NiftyregInputMaskInterface<  unsigned char  > >,
  Providing< NiftyregWarpedImageInterface< TPixel >, NiftyregAffineMatrixInterface , UpdateInterface >
  >
{
public:

  /** Standard ITK typedefs. */
  typedef NiftyregAladinComponent< TPixel > Self;
  typedef SuperElastixComponent<
    Accepting< NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
               NiftyregInputMaskInterface< unsigned char > >,
    Providing< NiftyregWarpedImageInterface< TPixel >, NiftyregAffineMatrixInterface, UpdateInterface >
    >                                      Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  NiftyregAladinComponent( const std::string & name, LoggerImpl & logger );
  ~NiftyregAladinComponent();

  int Accept( typename NiftyregReferenceImageInterface< TPixel >::Pointer ) override;
  int Accept(typename NiftyregFloatingImageInterface< TPixel >::Pointer) override;
  int Accept( typename NiftyregInputMaskInterface< unsigned char >::Pointer ) override;

  std::shared_ptr< nifti_image > GetWarpedNiftiImage() override;

  mat44 * GetAffineNiftiMatrix() override;

  void Update() override;
  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  static const char * GetDescription() { return "NiftyregAladin Component"; }

  bool ConnectionsSatisfied() override;

private:

  reg_aladin< TPixel > *         m_reg_aladin;
  std::shared_ptr< nifti_image > m_reference_image;
  std::shared_ptr< nifti_image > m_floating_image;
  std::shared_ptr< nifti_image > m_warped_image;
  std::shared_ptr< nifti_image > m_input_mask;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "NiftyregAladinComponent" }, { keys::PixelType, PodString< TPixel >::Get() } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftyregAladinComponent.hxx"
#endif
#endif // #define selxNiftyregAladinComponent_h
