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

#ifndef selxNiftyregReadImageComponent_h
#define selxNiftyregReadImageComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include <string.h>
//#include "_reg_f3d.h"
#include "_reg_ReadWriteImage.h"
#include "_reg_ReadWriteMatrix.h"

namespace selx
{
template< class TPixel >
class NiftyregReadImageComponent :
  public SuperElastixComponent<
  Accepting<  >,
  Providing< NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >>
  >
{
public:

  /** Standard ITK typedefs. */
  typedef NiftyregReadImageComponent< TPixel > Self;
  typedef ComponentBase                        Superclass;
  typedef std::shared_ptr< Self >              Pointer;
  typedef std::shared_ptr< const Self >        ConstPointer;

  NiftyregReadImageComponent( const std::string & name );
  virtual ~NiftyregReadImageComponent();

  virtual nifti_image * GetFloatingNiftiImage() override;

  virtual nifti_image * GetReferenceNiftiImage() override;

  virtual bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;

  static const char * GetDescription() { return "NiftyregReadImage Component"; }

private:

  std::string   m_ImageFileName;
  nifti_image * m_referenceImage;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "NiftyregReadImageComponent" }, { keys::PixelType, PodString< TPixel >::Get() }, { keys::Dimensionality, "3" } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxNiftyregReadImageComponent.hxx"
#endif
#endif // #define selxNiftyregReadImageComponent_h
